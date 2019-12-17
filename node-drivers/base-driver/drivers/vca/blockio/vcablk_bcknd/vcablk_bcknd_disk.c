/*
 * Intel VCA Software Stack (VCASS)
 *
 * Copyright(c) 2016-2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Intel VCA Block IO driver.
 */
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/delay.h>

#ifdef TEST_BUILD
#include "vcablk_test_hw_ops.h"
#else
#include "plx_hw_ops_blockio.h"
#endif

#include "vcablk_common/vcablk_common.h"
#include "vcablk_bcknd_hal.h"
#include "vcablk_bcknd_disk.h"
#include "vcablk_bcknd_media.h"
#include "../vcablk/vcablk_pool.h"

//#define BLOCKIO_FORCE_MEMCPY
//#define BLOCKIO_FORCE_DMA_SYNC

#define VCABLK_MAX_PARTS_PER_REQUEST	256
#define TIMEOUT_POOL_POP_MS		10000

typedef struct {
	void *buffer;
	size_t buffer_size;
	dma_addr_t buffer_phys_da;
} vcablk_buffer;

typedef struct {
	struct vcablk_bcknd_disk *bckd;
	unsigned long sector;
	unsigned long nsec;
	void *remapped;
	int response_cookie;
} callback_param_t;

typedef struct {
	callback_param_t callback_param;
	vcablk_buffer buffer;
	/* Keep to close waiting callback before deinit DMA engine. */
	struct dma_async_tx_descriptor *tx;
} transfer_parameters_t;

/*
 * The internal representation of our bcknd device.
 */
struct vcablk_bcknd_disk {
	struct vcablk_bcknd_dev *bdev;
	int bcknd_id;
	unsigned sectors_num;		/* Number of linear sectors */
	int hardsect_size;		/* Size of disk sector */
	enum disk_state state;
	struct vcablk_media *media;

	wait_queue_head_t probe_queue;

	/* Request IRQ */
	int request_db;
	void *request_irq;
	struct task_struct *request_process_thread;
	wait_queue_head_t request_wq;

	/* Access to front page */
	int done_db;
	struct vcablk_ring *request_ring;
	__u16 request_ring_nums;
	struct vcablk_ring *completion_ring;
	__u16 completion_ring_num_elems;

	/* hw access */
	struct plx_blockio_hw_ops* hw_ops;

	__u16 request_last_used;

	struct vcablk_request request_buff[VCABLK_MAX_PARTS_PER_REQUEST];

	vcablk_pool_t *transfer_buffer_pool;
	wait_queue_head_t transfer_buffer_pool_wq;

};

void vcablk_bcknd_buffer_deinit(vcablk_pool_t *vcablk_buffer_pool, struct dma_chan *dma_ch)
{
	vcablk_buffer *buffer;
	void* ptr = NULL;
	int iter = 0;
	vcablk_pool_foreach_all(ptr, vcablk_buffer_pool, iter) {
		transfer_parameters_t *data = (transfer_parameters_t*)ptr;
		if(data->tx) {
			/* Disable DMA Async Callbacks from cleanup DMA, when transfer
			 * not finished correct (dma hang etc.) */
			data->tx->callback = NULL;
			data->tx->callback_param = NULL;
			data->tx = NULL;
		}
		buffer = &data->buffer;
		if (buffer->buffer) {
			if (buffer->buffer_phys_da && dma_ch) {
				dma_unmap_single(dma_ch->device->dev,
						buffer->buffer_phys_da,
						buffer->buffer_size,
						DMA_BIDIRECTIONAL);

				buffer->buffer_phys_da = 0;
			}
			free_pages((unsigned long)buffer->buffer,
					get_order(buffer->buffer_size));
			buffer->buffer = NULL;
		}
	}
}

int vcablk_bcknd_buffer_init(struct dma_chan *dma_ch, vcablk_pool_t *vcablk_buffer_pool)
{
	/* Always use intermediate buffer */
	vcablk_buffer *buffer;
	void *ptr = NULL;
	int iter;

	if (!dma_ch) {
		printk(KERN_DEBUG "%s: Use MEMCPY buffer\n", __func__);
	}

	vcablk_pool_foreach_all(ptr, vcablk_buffer_pool, iter) {
		transfer_parameters_t *data = (transfer_parameters_t*)ptr;
		buffer = &data->buffer;

		buffer->buffer = NULL;
		buffer->buffer_size = 0;
		buffer->buffer_phys_da = 0;

		buffer->buffer_size = PAGE_SIZE;
		BUG_ON(!(buffer->buffer_size >> SECTOR_SHIFT));
		buffer->buffer = (void *)__get_free_pages(GFP_KERNEL | GFP_DMA,
				get_order(buffer->buffer_size));
		if (!buffer->buffer)
			goto error;

		if (dma_ch) {
			buffer->buffer_phys_da = dma_map_single(
				dma_ch->device->dev,
				buffer->buffer,
				buffer->buffer_size,
				DMA_BIDIRECTIONAL);
			if (dma_mapping_error(dma_ch->device->dev, buffer->buffer_phys_da)) {
				buffer->buffer_phys_da = 0;
				goto error;
			}
		}
	}
	return 0;
error:
	vcablk_bcknd_buffer_deinit(vcablk_buffer_pool, dma_ch);
	return -EIO;
}

static int
vcablk_bcknd_disk_send_response(struct vcablk_bcknd_disk *bckd,
		__u16 cookie, int ret)
{
	int err = 0;
	struct vcablk_ring *completion_ring = (struct vcablk_ring *)bckd->completion_ring;
	__u16 last_add = completion_ring->last_add;
	struct vcablk_completion *ack;
	int sleep_counter = 200; /* Wait 2 seconds */

	while (VCA_RB_BUFF_FULL(last_add, completion_ring->last_used,
			bckd->completion_ring_num_elems)) {
		if (--sleep_counter <= 0) {
			printk(KERN_ERR "%s: ERROR Timeout Ack ring full! cookie %u, ret %i "
					"num_elems %u completion_ring->last_add %u last_use %u last_add %u\n",
					__func__, cookie, ret, bckd->completion_ring_num_elems, completion_ring->last_add,
					completion_ring->last_used, last_add);
			return -EIO;
		}
		msleep(10);
	}

	pr_debug("%s:  cookie %u, ret %i \n", __func__, cookie, ret);

	ack = VCABLK_RB_GET_COMPLETION(last_add, bckd->completion_ring_num_elems, completion_ring->elems);
	/* Write to ACK buffer */
	iowrite8((__s8)ret, &ack->ret);
	iowrite16(cookie, &ack->cookie);
	wmb();
	last_add = VCA_RB_COUNTER_ADD(last_add, 1, bckd->completion_ring_num_elems);
	iowrite16(last_add, &completion_ring->last_add);
	wmb();

	/* Send IRQ done */
	bckd->hw_ops->send_intr(bckd->bdev->mdev.parent, bckd->done_db);

	return err;
}

static void vca_bcknd_callback_write(void* arg)
{
	int err = 0;
	transfer_parameters_t *dma_args = (transfer_parameters_t*)arg;
	callback_param_t *data = &dma_args->callback_param;
	vcablk_buffer *buffer = &dma_args->buffer;
	pr_debug("%s: buffer:%p, sector:%lu, response_cookie %i", __func__, buffer->buffer, data->sector, data->response_cookie);

	dma_args->tx = NULL;

	err = vcablk_media_transfer(data->bckd->media, data->sector, data->nsec, buffer->buffer, 1);
	if(err) {
		printk(KERN_ERR "%s: Can not write data buffer, after DMA write: sector: %lu",
				__func__, data->sector);
	}

	if(data->remapped) {
		data->bckd->bdev->hw_ops->iounmap(data->bckd->bdev->mdev.parent, data->remapped);
		data->remapped = NULL;
	}

	if (data->response_cookie >= 0)
		vcablk_bcknd_disk_send_response(data->bckd, data->response_cookie, err);

	vcablk_pool_push(data->bckd->transfer_buffer_pool, arg);
	wake_up(&data->bckd->transfer_buffer_pool_wq);
}

static void vca_bcknd_callback_read(void* arg)
{
	transfer_parameters_t *dma_args = (transfer_parameters_t*)arg;
	callback_param_t *data = &dma_args->callback_param;
	pr_debug("%s: cookie:%d", __func__, data->response_cookie);

	dma_args->tx = NULL;

	if(data->remapped) {
		data->bckd->bdev->hw_ops->iounmap(data->bckd->bdev->mdev.parent, data->remapped);
		data->remapped = NULL;
	}

	if (data->response_cookie >= 0)
		vcablk_bcknd_disk_send_response(data->bckd, data->response_cookie, 0);

	vcablk_pool_push(data->bckd->transfer_buffer_pool, arg);
	wake_up(&data->bckd->transfer_buffer_pool_wq);
}

static transfer_parameters_t
*vcablk_bcknd_get_transfer_param(struct vcablk_bcknd_disk *bckd)
{
	transfer_parameters_t *result = NULL;
	while(result == NULL) {
		result = vcablk_pool_pop(bckd->transfer_buffer_pool, NULL);
		if (result != NULL) {
			BUG_ON(result->tx);
			break;
		}
		if (!wait_event_interruptible_timeout(
				bckd->transfer_buffer_pool_wq,
				vcablk_is_available(bckd->transfer_buffer_pool),
				msecs_to_jiffies(TIMEOUT_POOL_POP_MS))) {
			break;
		}
	}
	return result;
}

static int
vcablk_bcknd_transfer_device(struct vcablk_bcknd_disk *bckd,
		transfer_parameters_t *transfer_params,
		void *remapped,
		unsigned long bytes,
		int write)
{
	int ret = 0;
	dma_async_tx_callback callback;
	if (write) {
		callback = vca_bcknd_callback_write;
	} else {
		callback = vca_bcknd_callback_read;
	}

	if (!bckd->bdev->dma_ch) {
		/* MEMCPY */
		if (write) {
			memcpy_fromio(transfer_params->buffer.buffer, remapped, bytes);
		} else {
			memcpy_toio(remapped, transfer_params->buffer.buffer, bytes);
			wmb();
			ioread8(remapped + bytes -1);
		}
		callback(transfer_params);
	} else {
#ifdef BLOCKIO_FORCE_DMA_SYNC
		/* DMA SYNC */
		if (write) {
			ret = vcablk_bcknd_dma_sync(bckd->bdev->dma_ch,
					transfer_params->buffer.buffer_phys_da,
					bckd->bdev->hw_ops->bar_va_to_pa(bckd->bdev->mdev.parent, remapped),
					bytes);
		} else {
			ret = vcablk_bcknd_dma_sync(bckd->bdev->dma_ch,
					bckd->bdev->hw_ops->bar_va_to_pa(bckd->bdev->mdev.parent, remapped),
					transfer_params->buffer.buffer_phys_da,
					bytes);
		}

		if (!ret) {
			callback(transfer_params);
		}
#else /* BLOCKIO_FORCE_DMA_SYNC */
		/* DMA ASYNC */
		dma_cookie_t dma_cookie = 0;

		BUG_ON(transfer_params->tx);

		if (write) {
			dma_cookie = vcablk_bcknd_dma_async(bckd->bdev->dma_ch,
					transfer_params->buffer.buffer_phys_da,
					bckd->bdev->hw_ops->bar_va_to_pa(bckd->bdev->mdev.parent, remapped),
					bytes, callback, transfer_params, &transfer_params->tx);
		} else {
			dma_cookie = vcablk_bcknd_dma_async(bckd->bdev->dma_ch,
					bckd->bdev->hw_ops->bar_va_to_pa(bckd->bdev->mdev.parent, remapped),
					transfer_params->buffer.buffer_phys_da,
					bytes, callback, transfer_params, &transfer_params->tx);
		}

		if (dma_submit_error(dma_cookie)) {
			printk(KERN_ERR "%s: DMA transfer error %i Backend %i "
					"DMA error occurred, dma cookie: %d.\n",
					__func__, ret, bckd->bcknd_id, dma_cookie);
			ret = -ENOMEM;
		}
#endif /* BLOCKIO_FORCE_DMA_SYNC */
	}
	return ret;
}

int vcablk_bcknd_transfer(struct vcablk_bcknd_disk *bckd,
		unsigned long sector,
		unsigned long sectors_num,
		__u64 phys_buff,
		int write,
		int response_cookie)
{
	int ret = 0;
	uintptr_t offset_ptr;

	void *remapped = bckd->bdev->hw_ops->ioremap(bckd->bdev->mdev.parent,
		phys_buff, sectors_num << SECTOR_SHIFT);

	if (!remapped) {
		printk(KERN_ERR "%s: Can not remap memory: phys_buff %llu, "
				"sector %lu, sectors_num %lu\n",
				__func__, phys_buff, sector, sectors_num);
		return -EIO;
	}

	offset_ptr = (uintptr_t)remapped;
	while (sectors_num) {
		unsigned long nsec;
		unsigned long bytes;
		dma_cookie_t dma_cookie = 0;
		transfer_parameters_t *transfer_params = vcablk_bcknd_get_transfer_param(bckd);
		if (!transfer_params) {
			printk(KERN_ERR "%s: Can not get dma_param, request type %i,"
					"TIMEOUT on dma_buffer_pool, request_cookie: %i\n",
					__func__, write, response_cookie);
			ret = -ENOMEM;
			break;
		}

		nsec = min(sectors_num, transfer_params->buffer.buffer_size >> SECTOR_SHIFT);
		bytes = nsec << SECTOR_SHIFT;
		sectors_num -= nsec;

		if (!sectors_num) {
			/* Last buffer */
			transfer_params->callback_param.response_cookie = response_cookie;
			transfer_params->callback_param.remapped = remapped;
		} else {
			transfer_params->callback_param.response_cookie = -1;
			transfer_params->callback_param.remapped = NULL;
		}
		transfer_params->callback_param.bckd = bckd;
		transfer_params->callback_param.sector = sector;
		transfer_params->callback_param.nsec = nsec;

		pr_debug("%s: phys_buff: %llu, sectors_num: %lu, nsec: %lu, bytes: %lu,"
				" write: %i, response_cookie %i", __func__, phys_buff,
				sectors_num, nsec, bytes, write,
				transfer_params->callback_param.response_cookie);

		if (!write) {
			ret = vcablk_media_transfer(bckd->media, sector, nsec,
					transfer_params->buffer.buffer, write);
			if (ret) {
				printk(KERN_ERR "%s: error in vcablk_media_transfer: %d", __func__, ret);
				break;
			}
		}

		ret = vcablk_bcknd_transfer_device(bckd, transfer_params,
				(void *)offset_ptr, bytes, write);

		if (ret) {
			printk(KERN_ERR "%s: DMA transfer error %i Backend %i "
					"response_cookie %i. DMA error occurred, dma cookie: %d.\n",
					__func__, ret, bckd->bcknd_id, response_cookie, dma_cookie);
			break;
		}

		sector += nsec;
		offset_ptr += bytes;
	}

	if (ret) {
		/* When exit with error, some started transfer by DMA can still
		 * work in background! */
		printk(KERN_ERR "%s: DMA transfer error in background! ret %i\n",
				__func__, ret);
		bckd->bdev->hw_ops->iounmap(bckd->bdev->mdev.parent, remapped);
	}
	return ret;
}

enum disk_state
vcablk_bcknd_disk_get_state(struct vcablk_bcknd_disk *bckd)
{
	return bckd->state;
}

const struct vcablk_media*
vcablk_bcknd_disk_get_media(struct vcablk_bcknd_disk *bckd)
{
	return bckd->media;
}

const int
vcablk_bcknd_disk_get_id(struct vcablk_bcknd_disk *bckd)
{
	return bckd->bcknd_id;
}

wait_queue_head_t *
vcablk_bcknd_disk_get_probe_queue(struct vcablk_bcknd_disk *bckd)
{
	return &bckd->probe_queue;
}

static int
vcablk_bcknd_disk_request_step(struct vcablk_bcknd_disk *bckd, struct vcablk_request *req, int response_cookie)
{
	int ret = 0;
	int write = 0;

	switch (req->request_type) {
	case REQUEST_SYNC:
		printk(KERN_WARNING "%s: SYNC not implemented for ASYNC DMA!\n", __func__);
		ret = vcablk_media_sync(bckd->media);
		break;
	case REQUEST_WRITE:
		write = 1;
	case REQUEST_READ: {
		ret = vcablk_bcknd_transfer(
				bckd,
				req->sector,
				req->sectors_num,
				req->phys_buff,
				write,
				response_cookie);
		break;
	}
	case 0: /*Queue is empty*/
	default:
		pr_err("%s: Unknown request type: phys_buff %llu, "
				"sector %llu, sectors_num %llu, cookie %u, request_type %u\n",
				__func__, req->phys_buff, req->sector, req->sectors_num,
				req->cookie, req->request_type);
		ret = -EIO;
	}
	return ret;
}

static int
vcablk_bcknd_disk_request(struct vcablk_bcknd_disk *bckd,
		struct vcablk_request *request_buff, __u16 size)
{
	int err = 0;
	struct vcablk_request *request;
	__u16 cookie = 0;
	__u16 id = 0;
	int response_cookie = -1;

	if (!size) {
		pr_err("%s: Request is empty!\n", __func__);
		return -EIO;
	}

	for (id = 0; id < size; ++id) {
		request = request_buff + id;
		if (!cookie) {
			cookie = request->cookie;
		}

		if (cookie != request->cookie) {
			pr_err("%s: Request error %i Backend %i "
				"Incorrect cookie %i expected %i request %i/%i\n", __func__,
				err, bckd->bcknd_id, request->cookie, cookie, id, size);
			err = -EIO;
			/* Ignore requests to next cookie */
			break;
		}
		pr_debug("%s: cookie %u request %i/%i sector %lu, sectors %lu, "
			"physBuff 0x%p, req_type %i\n", __func__, cookie, id, size,
			(size_t)request->sector, (size_t)request->sectors_num,
			(void*)request->phys_buff, request->request_type);


		if (id + 1 >= size) {
			response_cookie = cookie;
		}

		err = vcablk_bcknd_disk_request_step(bckd, request, response_cookie);
		if (err) {
			printk(KERN_ERR "%s: Request error %i Backend %i cookie %i\n",
				__func__, err, bckd->bcknd_id, cookie);
			/* Ignore requests to next cookie */
			break;
		}
	}

	if (err)
		vcablk_bcknd_disk_send_response(bckd, cookie, err);

	return err;
}

/* Get next single request form requests ring.
 * RETURN: Number of elements in request, if not request then return 0*/
static __u16
vcablk_bcknd_disk_get_next_request(struct vcablk_bcknd_disk *bckd,
		struct vcablk_request *request_buff)
{
	struct vcablk_ring *ring_req = bckd->request_ring;
	__u16 size = 0;
	__u16 last_add = ring_req->last_add;
	__u16 last_used = bckd->request_last_used;
	__u16 cookie = 0;

	if (last_used != last_add) {
		for (;last_used != last_add && size < VCABLK_MAX_PARTS_PER_REQUEST;
			last_used = VCA_RB_COUNTER_ADD(last_used, 1, bckd->request_ring_nums)) {

			/* Copy remote request to local buffer. */
			struct vcablk_request *request_remote = VCABLK_RB_GET_REQUEST(last_used, bckd->request_ring_nums, ring_req->elems);
			struct vcablk_request *request_local = request_buff + size;
			memcpy_fromio(request_local, request_remote, sizeof(struct vcablk_request));
			rmb();

			if (!cookie) {
				/* New cookie */
				cookie = request_local->cookie;
				/* Ignore invalid cookies */
				if (!cookie) {
					pr_err("%s: Invalid request cookie 0\n", __func__);
					continue;
				}
			}

			if (cookie != request_local->cookie) {
				/* Not increment last_use in loop,
				 * next time will start from this same value. */
				break;
			}
			++size;
		}

		/* First clean request ring, before execute response! */
		bckd->request_last_used = last_used;
		iowrite16(last_used, &bckd->request_ring->last_used);
	}
	return size;
}

static int
vcablk_bcknd_disk_make_request_thread(void *data)
{
	struct vcablk_bcknd_disk *bckd = data;
	struct vcablk_ring *ring_req = bckd->request_ring;
	struct vcablk_request *request_buff = bckd->request_buff;
	__u16 request_size;
	__u16 last_add;

	pr_debug("%s: Thread start dev_id %i\n", __func__, bckd->bcknd_id);

	while (!kthread_should_stop()) {
		wait_event_interruptible(bckd->request_wq,
				ring_req->last_add != bckd->request_last_used ||
				kthread_should_stop());

		if (bckd->state != DISK_STATE_OPEN || kthread_should_stop()) {
			pr_debug("%s: Thread start dev_id %i "
					"DISK NOT OPEN or stop\n", __func__, bckd->bcknd_id);
			continue;
		}

		/*
		 * Finish all available requests in ring, before check bckd->request_wq
		 * to not stack on queue.Can not pool on ring_req->last_add and finish
		 * all incoming requests, to avoid OS report: lock CPU.
		 * */
		last_add = ring_req->last_add;
		while (last_add != bckd->request_last_used && !kthread_should_stop()) {
			request_size = vcablk_bcknd_disk_get_next_request(bckd, request_buff);
			if (request_size)
				vcablk_bcknd_disk_request(bckd, request_buff, request_size);
		}
	}

	pr_debug("%s: Thread STOP dev_id %i\n", __func__, bckd->bcknd_id);

	do_exit(0);
	return 0;
}

static irqreturn_t
vcablk_bcknd_disk_request_irq(int irq, void *data)
{
	struct vcablk_bcknd_disk *bckd = (struct vcablk_bcknd_disk *)data;

	if (bckd) {
		if (bckd->state == DISK_STATE_OPEN) {
			wake_up(&bckd->request_wq);
		} else {
			pr_err("%s: Ignore Request IRQ %i dev not ready\n",
					__func__, bckd->bcknd_id);
		}

		bckd->hw_ops->ack_interrupt(bckd->bdev->mdev.parent,
				bckd->request_db);
	} else {
		pr_err("%s: unknown IRQ \n", __func__);
	}

	return IRQ_HANDLED;
}

void
vcablk_bcknd_disk_stop(struct vcablk_bcknd_disk *bckd)
{
	pr_debug("%s stop %i\n", __func__, bckd->bcknd_id);
	if (bckd->state == DISK_STATE_OPEN) {
		/* TODO: wait for thread stop? */
		if (bckd->request_process_thread) {
			kthread_stop(bckd->request_process_thread);
			bckd->request_process_thread = NULL;
			wake_up(&bckd->request_wq);
		}
		if (bckd->request_db >= 0) {
			bckd->hw_ops->free_irq(bckd->bdev->mdev.parent, bckd->request_irq, bckd);
			bckd->request_db = -1;
			bckd->request_irq = NULL;
		}
		bckd->done_db = -1;

		if (bckd->request_ring) {
			bckd->bdev->hw_ops->iounmap(bckd->bdev->mdev.parent, bckd->request_ring);
			bckd->request_ring = NULL;
		}
		if (bckd->completion_ring) {
			bckd->bdev->hw_ops->iounmap(bckd->bdev->mdev.parent,bckd->completion_ring);
			bckd->completion_ring = NULL;
		}
		bckd->state = DISK_STATE_CREATE;
	}
}

void
vcablk_bcknd_disk_destroy(struct vcablk_bcknd_disk *bckd)
{
	pr_info("%s start %i\n", __func__, bckd->bcknd_id);
	vcablk_bcknd_disk_stop(bckd);

	vcablk_bcknd_buffer_deinit(bckd->transfer_buffer_pool, bckd->bdev->dma_ch);

	if (bckd->media) {
		vcablk_media_destroy(bckd->media);
		bckd->media = NULL;
	}

	vcablk_pool_deinit(bckd->transfer_buffer_pool);
	kfree(bckd);
	bckd = NULL;

	pr_info("%s end\n", __func__);
}

/*
 * Set up our internal device.
 * size - Size of device in bytes
 */
struct vcablk_bcknd_disk*
vcablk_bcknd_disk_create(struct vcablk_bcknd_dev *bdev,
		struct vcablk_disk_open_desc *desc)
{
	int err = 0;
	struct vcablk_bcknd_disk *bckd = NULL;
	bool read_only;
	size_t size_bytes;

	pr_debug(KERN_ERR "%s disk_id %i\n", __func__, desc->disk_id);

#ifdef BLOCKIO_FORCE_MEMCPY
	bdev->dma_ch = NULL;
	pr_err(KERN_ERR "%s BLOCKIO_FORCE_MEMCPY\n", __func__);
#else /* BLOCKIO_FORCE_MEMCPY */
#ifdef BLOCKIO_FORCE_DMA_SYNC
	pr_err(KERN_ERR "%s BLOCKIO_FORCE_DMA_SYNC\n", __func__);
#endif /* BLOCKIO_FORCE_DMA_SYNC */
#endif /* BLOCKIO_FORCE_MEMCPY */

	bckd = kmalloc(sizeof (struct vcablk_bcknd_disk), GFP_KERNEL);
	if (bckd == NULL) {
		pr_err("%s: Can not allocate device size %lu\n", __func__,
				sizeof (struct vcablk_bcknd_disk));
		err = -ENOMEM;
		goto exit;
	}
	memset (bckd, 0, sizeof (struct vcablk_bcknd_disk));

	bckd->transfer_buffer_pool = vcablk_pool_init(VCABLK_MAX_PARTS_PER_REQUEST, sizeof(transfer_parameters_t));
	if (bckd->transfer_buffer_pool == NULL) {
		printk(KERN_ERR "%s: Can not allocate memory poll size %lu\n", __func__,
				(VCABLK_MAX_PARTS_PER_REQUEST + 1) * sizeof(transfer_parameters_t));
		err = -ENOMEM;
		goto exit;
	}
	init_waitqueue_head(&bckd->transfer_buffer_pool_wq);

	bckd->bdev = bdev;
	bckd->hw_ops = bdev->hw_ops;
	bckd->request_db = -1;
	bckd->bcknd_id = desc->disk_id;
	bckd->state = DISK_STATE_CREATE;
	read_only = (desc->mode == VCABLK_DISK_MODE_READ_ONLY);

	/* Set size of disk */
	bckd->hardsect_size = SECTOR_SIZE; /* Base size of block */
	bckd->sectors_num = desc->size / bckd->hardsect_size;
	size_bytes = (size_t)bckd->sectors_num * bckd->hardsect_size;
	if (desc->size % bckd->hardsect_size) {
		pr_warning("%s: size %lu is not multiple of sector size %i\n",
				__func__, size_bytes, bckd->hardsect_size);
	}
	pr_debug("%s disk_id %i, size_org %llu, size_bytes %lu, "
			"sectors_num %u,hardsect_size %u, path %s\n", __func__,
			bckd->bcknd_id, (u64)desc->size, size_bytes, bckd->sectors_num,
			bckd->hardsect_size, desc->file_path);

	desc->file_path[sizeof(desc->file_path) - 1] = '\0';

	if (desc->type == VCABLK_DISK_TYPE_FILE) {
		bckd->media = vcablk_media_create_file(size_bytes,
				desc->file_path, read_only);
	} else if (desc->type == VCABLK_DISK_TYPE_MEMORY) {
		bckd->media = vcablk_media_create_memory(size_bytes,
				desc->file_path, read_only);
	}
	if (IS_ERR(bckd->media)) {
		pr_err("%s: open file failure, type %i\n",
				__func__, desc->type);
		err = PTR_ERR(bckd->media);
		bckd->media = NULL;
		goto err;
	}

	err = vcablk_bcknd_buffer_init(bdev->dma_ch, bckd->transfer_buffer_pool);
	if (err) {
		pr_err("%s: can not init buffer err %i\n",
				__func__, err);
		goto err;
	}

	init_waitqueue_head(&bckd->request_wq);
	init_waitqueue_head(&bckd->probe_queue);

	return bckd;

err:
	vcablk_bcknd_buffer_deinit(bckd->transfer_buffer_pool, bckd->bdev->dma_ch);

	if (bckd->media) {
		vcablk_media_destroy(bckd->media);
		bckd->media = NULL;
	}
exit:
	if (bckd)
		kfree(bckd);
	bckd = NULL;
	return  ERR_PTR(err);
}

int
vcablk_bcknd_disk_prepare_request_db(struct vcablk_bcknd_disk *bckd)
{
	pr_debug("%s: dev_id %i!!!\n", __func__, bckd->bcknd_id);

	if (bckd->request_db >= 0) {
		pr_err("%s: Disk ID %i Previous doorbell %i not released!\n",
						__func__, bckd->bcknd_id, bckd->request_db);
	}

	bckd->request_db = bckd->bdev->hw_ops->next_db(bckd->bdev->mdev.parent);
	if (bckd->request_db < 0) {
		pr_err("%s: Disk with ID %i Can not get doorbell\n",
				__func__, bckd->bcknd_id);
	}

	return bckd->request_db;
}


int
vcablk_bcknd_disk_start(struct vcablk_bcknd_disk *bckd, int done_db,
		__u64 request_ring_ph, __u64 request_ring_size,
		__u64 completion_ring_ph, __u64 completion_ring_size)
{
	int err = 0;
	pr_debug("%s: dev_id %i!!!\n", __func__, bckd->bcknd_id);
	if (bckd->state != DISK_STATE_CREATE) {
		pr_err("%s: Can not run thread, unexpected state %i!!!\n",
				__func__, bckd->state);
		err = -EIO;
		goto exit;
	}

	if (bckd->request_db < 0) {
		pr_err("%s: Disk with ID %i Incorrect doorbell\n",
				__func__, bckd->bcknd_id);
		err = -EBUSY;
		goto exit;
	}

	bckd->request_ring = bckd->bdev->hw_ops->ioremap(bckd->bdev->mdev.parent,
			request_ring_ph, request_ring_size);
	if (!bckd->request_ring) {
		pr_err("%s: error remapping request ring\n", __func__);
		err = -EIO;
		goto err;
	}

	bckd->completion_ring = bckd->bdev->hw_ops->ioremap(bckd->bdev->mdev.parent,
			completion_ring_ph, completion_ring_size);
	if (!bckd->completion_ring) {
		pr_err("%s: error remapping request ring\n", __func__);
		err = -EIO;
		goto err;
	}

	if (!is_power_of_2(bckd->request_ring->num_elems)
			|| !is_power_of_2(bckd->completion_ring->num_elems)) {
			pr_err("%s: Rings request and ack size need be power of 2 "
					"ring_request_num %u completion_ring_num %u\n",
					__func__, bckd->request_ring->num_elems,
					bckd->completion_ring->num_elems);
		err = -EIO;
		goto err;
	}

	bckd->request_ring_nums = bckd->request_ring->num_elems;
	bckd->completion_ring_num_elems = bckd->completion_ring->num_elems;
	bckd->done_db = done_db;
	bckd->request_last_used = bckd->request_ring->last_used;

	pr_debug( "%s: Register on db %i blockdev %i\n", __func__,
			bckd->request_db, bckd->bcknd_id);

	bckd->request_irq = bckd->hw_ops->request_irq(
			bckd->bdev->mdev.parent,
			vcablk_bcknd_disk_request_irq, "vcablk_disk_request_irq",
			bckd, bckd->request_db);

	if (IS_ERR(bckd->request_irq)) {
		bckd->request_irq = NULL;
		err = -EIO;
		goto err;
	}

	bckd->request_process_thread = kthread_create(
			vcablk_bcknd_disk_make_request_thread,
			bckd, "vcablk_task_request");
	if (IS_ERR(bckd->request_process_thread)) {
		pr_err("%s: Can not create thread %i\n", __func__,
				(int)PTR_ERR(bckd->request_process_thread));
		bckd->request_process_thread = NULL;
		err = -EAGAIN;
		goto err_irq;
	}

	bckd->state = DISK_STATE_OPEN;
	wake_up_process(bckd->request_process_thread);
	goto exit;

err_irq:
	if (bckd->request_db >= 0)
		bckd->hw_ops->free_irq(bckd->bdev->mdev.parent, bckd->request_irq, bckd);

err:
	bckd->request_db = -1;
	bckd->request_irq = NULL;
	bckd->done_db = -1;

	if (bckd->request_ring) {
		bckd->bdev->hw_ops->iounmap(bckd->bdev->mdev.parent, bckd->request_ring);
		bckd->request_ring = NULL;
	}
	if (bckd->completion_ring) {
		bckd->bdev->hw_ops->iounmap(bckd->bdev->mdev.parent,bckd->completion_ring);
		bckd->completion_ring = NULL;
	}

exit:
	return err;
}
