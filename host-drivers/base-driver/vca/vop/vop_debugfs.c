/*
 * Intel VCA Software Stack (VCASS)
 *
 * Copyright(c) 2015-2017 Intel Corporation.
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
 * Intel Virtio Over PCIe (VOP) driver.
 *
 */
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/dma-direction.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>

#include "vop_main.h"
#include "vop_common.h"
#include "vop_kvec_buff.h"

static int vop_dp_show(struct seq_file *s, void *pos)
{
	struct vca_device_desc *d;
	struct vca_device_ctrl *dc;
	struct vca_vqconfig *vqconfig;
	__u32 *features;
	__u8 *config;
	struct vop_info *vi = s->private;
	struct vop_device *vpdev = vi->vpdev;
	struct vca_bootparam *bootparam = vpdev->hw_ops->get_dp(vpdev);
	int j, k;

	if (!bootparam) {
		seq_printf(s, "bootparam is NULL\n");
		goto end;
	}

	seq_printf(s, "Bootparam: magic 0x%x\n",
		   bootparam->magic);
	seq_printf(s, "Bootparam: version_host 0x%x\n",
		   bootparam->version_host);
	seq_printf(s, "Bootparam: version_card 0x%x\n",
		   bootparam->version_card);
	seq_printf(s, "Bootparam: h2c_config_db %d\n",
		   bootparam->h2c_config_db);
	seq_printf(s, "Bootparam: node_id %d\n",
		   bootparam->node_id);
	seq_printf(s, "Bootparam: c2h_scif_db %d\n",
		   bootparam->c2h_scif_db);
	seq_printf(s, "Bootparam: h2c_scif_db %d\n",
		   bootparam->h2c_scif_db);
	seq_printf(s, "Bootparam: scif_host_dma_addr 0x%llx\n",
		   bootparam->scif_host_dma_addr);
	seq_printf(s, "Bootparam: scif_card_dma_addr 0x%llx\n",
		   bootparam->scif_card_dma_addr);
	seq_printf(s, "Bootparam: test_host_dma_addr 0x%llx\n",
			   bootparam->test_host_dma_addr);
	seq_printf(s, "Bootparam: test_card_dma_addr 0x%llx\n",
			   bootparam->test_card_dma_addr);

	for (j = sizeof(*bootparam);
		j < VCA_DP_SIZE; j += vca_total_desc_size(d)) {
		d = (void *)bootparam + j;
		dc = (void *)d + vca_aligned_desc_size(d);

		/* end of list */
		if (d->type == 0)
			break;

		if (d->type == -1)
			continue;

		seq_printf(s, "Type %d ", d->type);
		seq_printf(s, "Num VQ %d ", d->num_vq);
		seq_printf(s, "Feature Len %d\n", d->feature_len);
		seq_printf(s, "Config Len %d ", d->config_len);
		seq_printf(s, "Shutdown Status %d\n", d->status);

		for (k = 0; k < d->num_vq; k++) {
			vqconfig = vca_vq_config(d) + k;
			seq_printf(s, "vqconfig[%d]: ", k);
			seq_printf(s, "address 0x%llx ",
				   vqconfig->address);
			seq_printf(s, "num %d ", vqconfig->num);
		}

		features = (__u32 *)vca_vq_features(d);
		seq_printf(s, "Features: Host 0x%x ", features[0]);
		seq_printf(s, "Guest 0x%x\n", features[1]);

		config = vca_vq_configspace(d);
		for (k = 0; k < d->config_len; k++)
			seq_printf(s, "config[%d]=%d\n", k, config[k]);

		seq_puts(s, "Device control:\n");
		seq_printf(s, "Config Change %d ", dc->config_change);
		seq_printf(s, "Vdev reset %d\n", dc->vdev_reset);
		seq_printf(s, "Guest Ack %d ", dc->guest_ack);
		seq_printf(s, "Host ack %d\n", dc->host_ack);
		seq_printf(s, "Used address updated %d ",
			   dc->used_address_updated);
		seq_printf(s, "Vdev 0x%llx\n", dc->vdev);
		seq_printf(s, "c2h conf doorbell %d ", dc->c2h_vdev_conf_db);
		seq_printf(s, "c2h avail doorbell %d ", dc->c2h_vdev_avail_db);
		seq_printf(s, "c2h used doorbell %d ", dc->c2h_vdev_used_db);
		seq_printf(s, "h2c conf doorbell %d\n", dc->h2c_vdev_conf_db);
		seq_printf(s, "h2c avail doorbell %d\n", dc->h2c_vdev_avail_db);
		seq_printf(s, "h2c used doorbell %d\n", dc->h2c_vdev_used_db);


	}

end:
	schedule_work(&vi->hotplug_work);
	return 0;
}

static int vop_dp_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, vop_dp_show, inode->i_private);
}

static const struct file_operations dp_ops = {
	.owner   = THIS_MODULE,
	.open    = vop_dp_debug_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static void vop_vring_info_show(struct seq_file *s, struct vring* v, const char * side, const char* name,  int avail_marker, int used_marker)
{
	unsigned int i;
	u16 avail_idx = v->avail ? v->avail->idx : 0;
	u16 used_idx = v->used ? v->used->idx : 0;

	seq_printf(s, "vring : %s %s\n", side, name);
	seq_printf(s, "num:%04x desc:%p avail.ring:%p used.ring:%p\n",
			v->num, v->desc, v->avail ? v->avail->ring : NULL, v->used ? v->used->ring : NULL);
	seq_printf(s, "avail.idx:%04x(%04x) used.idx: %04x(%04x)\n",
			avail_idx, avail_idx % v->num, used_idx, used_idx % v->num);

	if (!v->num)
		return;

	if (v->avail && v->avail->ring) {
		u16* ar = v->avail->ring;
		for (i=0; i<v->num / 8; i++) {
			seq_printf(s, "AVAIL[%04x:%04x] : %04x %04x %04x %04x %04x %04x %04x %04x",
					i*8, (i+1)*8-1,
					ar[i*8], ar[i*8+1], ar[i*8+2], ar[i*8+3], ar[i*8+4], ar[i*8+5], ar[i*8+6], ar[i*8+7]);
			if (avail_idx % v->num >= i*8 &&
			    avail_idx % v->num <  (i+1)*8)
				seq_printf(s, " <-");
			if (avail_marker != -1 &&
			    avail_marker % v->num >= i*8 &&
			    avail_marker % v->num <  (i+1)*8)
				seq_printf(s, " <-H");
			seq_printf(s, "\n");
		}
	}

	if (v->used && v->used->ring) {
		struct vring_used_elem* ur = v->used->ring;
		for (i=0; i<v->num / 8; i++) {
			seq_printf(s, "USED[%04x:%04x] : %04x(s:%04x) %04x(s:%04x) %04x(s:%04x) "
					"%04x(s:%04x) %04x(s:%04x) %04x(s:%04x) %04x(s:%04x) %04x(s:%04x)",
					i*8, (i+1)*8-1,
					ur[i*8].id, ur[i*8].len,
					ur[i*8+1].id, ur[i*8+1].len, ur[i*8+2].id, ur[i*8+2].len, ur[i*8+3].id, ur[i*8+3].len,
					ur[i*8+4].id, ur[i*8+4].len, ur[i*8+5].id, ur[i*8+5].len, ur[i*8+6].id, ur[i*8+6].len,
					ur[i*8+7].id, ur[i*8+7].len);
			if (used_idx % v->num >= i*8 &&
			    used_idx % v->num <  (i+1)*8)
				seq_printf(s, " <-");
			if (used_marker != -1 &&
			    used_marker % v->num >= i*8 &&
			    used_marker % v->num <  (i+1)*8)
				seq_printf(s, " <-H");
			seq_printf(s, "\n");
		}
	}

	if (v->desc) {
		 for (i=0; i<v->num ; i++) {
			 seq_printf(s, "DESC [%04x] : addr:%016llx len:%08x flags:%04x, next:%04x\n",
					 i, v->desc[i].addr, v->desc[i].len, v->desc[i].flags, v->desc[i].next);
		 }
	}
	seq_printf(s, "end vring : %s%s\n\n", side, name);
}

static void vop_vring_kvec_ring_show(struct seq_file *s, struct vop_dev_common *cdev,
	struct vop_kvec_ring *ring, const char* side, unsigned int idx)
{
	u32 send_max_size = 0;
	unsigned int i;

	if (!ring || !ring->buf) {
		seq_printf(s, "%s available kvec ring no %02i is NULL\n", side, idx);
		return;
	}

	if (ring->send_max_size) {
		send_max_size = *ring->send_max_size;
	}

	seq_printf(s, "%s available kvec ring no %02i num: 0x%08x send_max_size: %6u "
			"use: %05i last_cnt:0x%04x(0x%04x) current_cnt:0x%04x(0x%04x) stats: %u\n",
		side, idx, ring->num, send_max_size,
		KVEC_COUNTER_USED(ring->last_cnt, *ring->cnt ,ring->num),
		ring->last_cnt, KVEC_COUNTER_TO_IDX(ring->last_cnt, ring->num),
		*ring->cnt, KVEC_COUNTER_TO_IDX(*ring->cnt, ring->num),
		ring->stats_num);


	for (i=0; i<ring->num; i++) {
		seq_printf(s, "RING[%04x] : addr:%p len:%08llx head:%04x %s %s",
			i,
			ring->buf[i].iov.iov_base,
			(u64)ring->buf[i].iov.iov_len,
			ring->buf[i].head,
			ring->buf[i].flags & VOP_KVEC_FLAG_IN_USE ? "in_use" : "",
			ring->buf[i].flags & VOP_KVEC_FLAG_CANCELLED ? "cancelled" : "");
		if (i ==  KVEC_COUNTER_TO_IDX(ring->last_cnt, ring->num))
			seq_printf(s, " ->");
		if (i == KVEC_COUNTER_TO_IDX(*ring->cnt, ring->num))
			seq_printf(s, " <-");
		seq_printf(s, "\n");
	}
	seq_printf(s, "end of %s kvec ring no %02x\n\n", side, idx);
}

static void vop_vring_used_kvec_ring_show(struct seq_file *s, struct vop_dev_common *cdev,
	struct vop_used_kiov_ring *ring, const char* side)
{
	unsigned int i;

	if (!ring || !ring->buf) {
		seq_printf(s, "%s used kvec ring is NULL\n", side);
		return;
	}

	seq_printf(s, "%s used kvec ring no num: %08i use: %05i last_cnt:0x%04x(0x%04x) current_cnt:0x%04x(0x%04x)\n",
		side, ring->num, KVEC_COUNTER_USED(ring->last_cnt, *ring->cnt ,ring->num),
		ring->last_cnt, KVEC_COUNTER_TO_IDX(ring->last_cnt, ring->num),
		*ring->cnt, KVEC_COUNTER_TO_IDX(*ring->cnt, ring->num));
	for (i=0; i<ring->num; i++) {
		seq_printf(s, "USED[%04x] : head:%04x len:%08x",
			i,
			ring->buf[i].head,
			ring->buf[i].len);
		if (i ==  KVEC_COUNTER_TO_IDX(ring->last_cnt, ring->num))
			seq_printf(s, " ->");
		if (i == KVEC_COUNTER_TO_IDX(*ring->cnt, ring->num))
			seq_printf(s, " <-");
		seq_printf(s, "\n");
	}
	seq_printf(s, "end of %s kvec ring\n\n", side);
}

static void vop_vring_kvec_buf_show(struct seq_file *s, struct vop_dev_common *cdev)
{
	unsigned int i;

	vop_vring_used_kvec_ring_show(s, cdev, &cdev->kvec_buff.local_write_kvecs.used_ring, "local");
	vop_vring_used_kvec_ring_show(s, cdev, &cdev->kvec_buff.remote_write_kvecs.used_ring, "remote");

	for (i=0; i<KVEC_BUF_NUM; i++) {
		vop_vring_kvec_ring_show(s, cdev, cdev->kvec_buff.local_write_kvecs.rings + i, "local", i);
	}

	for (i=0; i<KVEC_BUF_NUM; i++) {
		vop_vring_kvec_ring_show(s, cdev, cdev->kvec_buff.remote_write_kvecs.rings + i, "remote", i);
	}
}

static void vop_cdev_items_show(struct seq_file *s, struct vop_dev_common *cdev)
{
	struct buffers_dma_ring *ring = cdev->buffers_ring;
	struct buffer_dma_item *item = NULL;
	unsigned int i;

	seq_printf(s, "transfer items ring buffer : rcv_idx: %08x dma_send:%04x transfer_done:%04x\n",
			ring->indicator_rcv, (u32)ring->counter_dma_send, (u32)ring->counter_done_transfer);
	for(i=0; i<VOP_RING_SIZE; i++) {
		item =  ring->items + i;
		seq_printf(s, "%04x %c src_ph:%016llx src_phys_da:%016llx src_phys_sz:%08x "
				"jiffies:%016llx head_from:%04x bytes_read:%08x data_size:%08x kvec_buf_id:%04x "
				"num_kvecs_to:%04x head_to:%04x bytes_written:%08x remapped:%p",
				item->id,
				item->status_ready ? 'R' : ' ',
				(u64)item->src_phys, (u64)item->src_phys_da, (u32)item->src_phys_sz,
				(u64)item-jiffies, item->head_from, (u32)item->bytes_read,
				(u32)item->data_size, item->kvec_buff_id,
				(u32)item->num_kvecs_to, item->head_to,
				(u32)item->bytes_written, item->remapped);
		if (i == ring->indicator_rcv)
			seq_printf(s, "<-R");
		seq_printf(s, "\n");
	}
}

static void vop_cdev_info_show(struct seq_file *s, struct vop_dev_common *cdev, bool host)
{
	struct vop_vringh *local_rx_vring = cdev->vringh_rcv;
	struct vop_vringh  *local_tx_vring = cdev->vringh_tx;
	const char* local = host ? "host" : "card";

	if (!local_rx_vring || !local_rx_vring) {
		seq_printf(s, "%s tx vringh: rings not ready local_rx_vring: %p "
				"local_tx_vring: %p\n", local,local_rx_vring, local_tx_vring);
		return;
	}

	/* vring and vringh for local TX queue */
	seq_printf(s, "%s tx vringh: last_avail_idx:%04x\n", local,
			local_tx_vring->vrh.last_avail_idx);


	vop_vring_info_show(s, &local_tx_vring->vrh.vring, local, "tx vring",
		 local_tx_vring->vrh.last_avail_idx, -1);

	/* vring for local RX queue */
	vop_vring_info_show(s, &local_rx_vring->vrh.vring, local, "rx vring", -1, -1);

	vop_vring_kvec_buf_show(s, cdev);

	vop_cdev_items_show(s, cdev);
}


/* kvec buffer info - this is a ring buffer used to provide write descriptors
 * from the receiver side to transmitter side. Local kvec buffer is where the
 * peer writes his writeable descriptors. Remote kvec buffer is where we
 * write write descriptors from local RX queue. */
static int vop_card_virtio_dev_info_show(struct seq_file *s, void *unused)
{
	struct vop_info *vi = s->private;
	struct list_head *pos, *tmp;
	struct vop_card_virtio_dev *vdev;

	mutex_lock(&vi->vop_mutex);

	if (vi->vpdev->dnode > 0) {
		/* host */
		list_for_each_safe(pos, tmp, &vi->vdev_list) {
			vdev = list_entry(pos, struct vop_card_virtio_dev, list);
			seq_printf(s, "VDEV type %d state %s in %ld out %ld\n",
				   vdev->virtio_id,
				   vop_vdevup(vdev) ? "UP" : "DOWN",
				   vdev->in_bytes,
				   vdev->out_bytes);
			{
				vop_cdev_info_show(s, &vdev->cdev, true);
			}
		}
	}
	else {
		/* card */
		list_for_each_safe(pos, tmp, &vi->vdev_list)  {
			struct  _vop_vdev *vpdev = list_entry(pos, struct _vop_vdev, list);
			vop_cdev_info_show(s, &vpdev->cdev, false);
		}

	}
	mutex_unlock(&vi->vop_mutex);

	return 0;
}

static int vop_card_virtio_dev_info_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, vop_card_virtio_dev_info_show, inode->i_private);
}

static const struct file_operations vdev_info_ops = {
	.owner   = THIS_MODULE,
	.open    = vop_card_virtio_dev_info_debug_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};


/* Debug hook to test lost node */
static void forever_ipi(void *ignored)
{
	while (1) {
		/* Spin forever */
	};
}

static int vop_crash_show(struct seq_file *s, void *pos)
{
	on_each_cpu(forever_ipi, NULL, 1);
	return 0;
}

static int vop_crash_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, vop_crash_show, inode->i_private);
}

static const struct file_operations crash_ops = {
	.owner   = THIS_MODULE,
	.open    = vop_crash_debug_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int vop_panic_show(struct seq_file *s, void *pos)
{
	panic("aiee");
	return 0;
}

static int vop_panic_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, vop_panic_show, inode->i_private);
}

static const struct file_operations panic_ops = {
	.owner   = THIS_MODULE,
	.open    = vop_panic_debug_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int vop_stat_debug_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static int vop_stat_debug_release(struct inode *inode, struct file *file)
{
	return 0;
}

static char * vop_stat_debug_show(struct vop_dev_common *cdev,
		char *tmp, char *end)
{
	int i;
	u32 send_max_size;

	tmp += snprintf(tmp, end - tmp, "ring prepare ");
	for (i=0; i<KVEC_BUF_NUM; ++i) {
		if (cdev->kvec_buff.remote_write_kvecs.rings) {
			tmp += snprintf(tmp, end - tmp, " [%i:(s%5u)]: %-12u", i,
				cdev->kvec_buff.remote_write_kvecs.rings[i].send_max_size_local,
				cdev->kvec_buff.remote_write_kvecs.rings[i].stats_num);
		} else {
			tmp += snprintf(tmp, end - tmp, " [%i:(s NULL)]:         NULL", i);
		}
	}
	tmp += snprintf(tmp, end - tmp, "\nring used    ");
	for (i=0; i<KVEC_BUF_NUM; ++i) {
		if (cdev->kvec_buff.local_write_kvecs.rings) {
			if (cdev->kvec_buff.local_write_kvecs.rings[i].send_max_size) {
				send_max_size =
					*cdev->kvec_buff.local_write_kvecs.rings[i].send_max_size;
			} else {
				send_max_size = 0;
			}
			tmp += snprintf(tmp, end - tmp, " [%i:(s%5u)]: %-12u", i,
					send_max_size,
					cdev->kvec_buff.local_write_kvecs.rings[i].stats_num);
		} else {
			tmp += snprintf(tmp, end - tmp, " [%i:(s NULL)]:         NULL", i);
		}
	}
	tmp += snprintf(tmp, end - tmp, "\n");
	return tmp;
}

static ssize_t vop_stat_debug_read(struct file *file,
		 char __user * buf, size_t count, loff_t * pos)
{
	unsigned size = 4096;
	char *tmp_buff = NULL;
	char *end;
	char *tmp;
	struct vop_info *vi = file->private_data;
	struct list_head *lpos, *ltmp;
	struct vop_dev_common *cdev;

	tmp_buff = kmalloc(size, GFP_KERNEL);
	if (!tmp_buff) {
		return 0;
	}

	end = tmp_buff + size;
	tmp = tmp_buff;

	mutex_lock(&vi->vop_mutex);
	list_for_each_safe(lpos, ltmp, &vi->vdev_list) {
		if (vi->vpdev->dnode > 0) {
			/* host */
			struct vop_card_virtio_dev *vdev =
					list_entry(lpos, struct vop_card_virtio_dev, list);
			tmp += snprintf(tmp, end - tmp, "HOST KVEC rings %u\n",
								KVEC_BUF_NUM);
			cdev = &vdev->cdev;
		} else {
			/* card */
			struct  _vop_vdev *vpdev = list_entry(lpos, struct _vop_vdev, list);
			tmp += snprintf(tmp, end - tmp, "CARD KVEC rings %u\n",
								KVEC_BUF_NUM);
			cdev = &vpdev->cdev;
		}
		tmp = vop_stat_debug_show(cdev, tmp, end);
	}
	mutex_unlock(&vi->vop_mutex);
	size = simple_read_from_buffer(buf, count, pos, tmp_buff, tmp - tmp_buff);
	kfree(tmp_buff);
	return size;
}

static ssize_t vop_stat_debug_write(struct file* file, const char* __user buff,
		size_t count, loff_t * ppos)
{
	struct vop_info *vi = file->private_data;
	struct list_head *lpos, *ltmp;
	struct vop_dev_common *cdev;
	int i;

	mutex_lock(&vi->vop_mutex);
	list_for_each_safe(lpos, ltmp, &vi->vdev_list) {
		if (vi->vpdev->dnode > 0) {
			/* host */
			struct vop_card_virtio_dev *vdev =
					list_entry(lpos, struct vop_card_virtio_dev, list);
			cdev = &vdev->cdev;

		} else {
			/* card */
			struct  _vop_vdev *vpdev = list_entry(lpos, struct _vop_vdev, list);
			cdev = &vpdev->cdev;
		}

		for (i=0; i<KVEC_BUF_NUM; ++i) {
			cdev->kvec_buff.remote_write_kvecs.rings[i].stats_num = 0;
			cdev->kvec_buff.local_write_kvecs.rings[i].stats_num = 0;
		}
	}
	mutex_unlock(&vi->vop_mutex);
	return count;
}

static const struct file_operations stats_ops = {
	.owner   = THIS_MODULE,
	.open    = vop_stat_debug_open,
	.read    = vop_stat_debug_read,
	.write   = vop_stat_debug_write,
	.release = vop_stat_debug_release
};

/*
 * async_dma - Wrapper for asynchronous DMAs.
 *
 * @dev - The address of the pointer to the device instance used
 * for DMA registration.
 * @dst - destination DMA address.
 * @src - source DMA address.
 * @len - size of the transfer.
 * @callback - routine to call after this operation is complete
 * @callback_param - general parameter to pass to the callback routine
 *
 * Return dma_cookie_t, check error by dma_submit_error(cookie)
 */
static dma_cookie_t vop_dma_async(struct vop_device *vpdev, dma_addr_t dst,
		dma_addr_t src, size_t len, dma_async_tx_callback callback,
		void *callback_param)
{
	dma_cookie_t cookie;
	struct dma_device *ddev;
	struct dma_async_tx_descriptor *tx;
	struct vop_info *vi = vpdev->priv;
	struct dma_chan *vop_ch = vi->dma_ch;
	unsigned long flags = DMA_PREP_INTERRUPT | DMA_PREP_FENCE;

	if (!vop_ch) {
		cookie = -EBUSY;
		goto error;
	}
	ddev = vop_ch->device;
	tx = ddev->device_prep_dma_memcpy(vop_ch, dst, src, len, flags);

	if (!tx) {
		cookie = -ENOMEM;
		goto error;
	} else {
		tx->callback = callback;
		tx->callback_param = callback_param;
		cookie = tx->tx_submit(tx);
		if (dma_submit_error(cookie)) {
			goto error;
		}
		dma_async_issue_pending(vop_ch);

		dev_dbg(&vi->vpdev->dev, "%s %d cookie %d, src 0x%llx, dst 0x%llx, "
				"len %lu\n", __func__, __LINE__, cookie, src, dst, len);
	}
error:
	if (dma_submit_error(cookie)) {
		dev_err(&vi->vpdev->dev, "%s %d err %d\n", __func__, __LINE__, cookie);
	}

	return cookie;
}

struct dmatest_done {
	bool                    done;
	wait_queue_head_t       *wait;
};

static void dmatest_callback(void *arg)
{
	struct dmatest_done *done = arg;

	done->done = true;
	wake_up_interruptible_all(done->wait);
}

static const size_t vop_dma_test_size = PAGE_SIZE;

static int vop_dma_test_show(struct seq_file *s, void *pos)
{
	struct vop_info *vi = s->private;
	struct vop_device *vpdev = vi->vpdev;
	bool link_side = vpdev->hw_ops->is_link_side(vpdev);
	struct vca_bootparam *bootparam = vpdev->hw_ops->get_dp(vpdev);
	__u64 *test_local_dma_addr;
	__u64 *test_remote_dma_addr;
	int	ret;

	if (!bootparam) {
		seq_printf(s, "bootparam is NULL\n");
		goto end;
	}

	seq_printf(s, "Bootparam: magic 0x%x\n", bootparam->magic);

	if (link_side) {
		seq_printf(s, "Card side\n");
		test_local_dma_addr = &bootparam->test_card_dma_addr;
		test_remote_dma_addr = &bootparam->test_host_dma_addr;
	} else {
		seq_printf(s, "Host side\n");
		test_local_dma_addr = &bootparam->test_host_dma_addr;
		test_remote_dma_addr = &bootparam->test_card_dma_addr;
	}

	if (!*test_local_dma_addr) {
		/* Allocate memory for local */
		unsigned long pages;
		dma_addr_t pa;
		pages = __get_free_pages(GFP_KERNEL | __GFP_ZERO | __GFP_DMA32,
						   get_order(vop_dma_test_size));
		if (!pages) {
			seq_printf(s, "failed to allocate write buffer, size %lu\n",
					vop_dma_test_size);
		} else {
			pa = dma_map_single(&vpdev->dev, (void*)pages, vop_dma_test_size,
					DMA_BIDIRECTIONAL);
			ret = dma_mapping_error(&vpdev->dev, pa);
			if (ret) {
				seq_printf(s, "failed to map write buffer %d, size %lu\n",
						   ret, vop_dma_test_size);
				free_pages(pages, get_order(vop_dma_test_size));
				pages = 0;
			}else {
				seq_printf(s, "Allocate memory to dma 0x%llx, size %lu\n",
						   pa, vop_dma_test_size);
				vi->dbg.dma_test_pages = pages;
				*test_local_dma_addr = pa;

			}
		}
	}

	seq_printf(s, "Bootparam: test_host_dma_addr 0x%llx\n",
			   bootparam->test_host_dma_addr);
	seq_printf(s, "Bootparam: test_card_dma_addr 0x%llx\n",
			   bootparam->test_card_dma_addr);
	seq_printf(s, "test_local_dma_addr: 0x%llx\n", *test_local_dma_addr);
	seq_printf(s, "test_remote_dma_addr: 0x%llx\n", *test_remote_dma_addr);

	if (vi->dbg.dma_test_pages) {
		/* Validate local memory */
		int i;
		char *addr = (char *)vi->dbg.dma_test_pages;
		char value = *addr;
		for (i = 0; i < vop_dma_test_size; ++i) {
			if (value != *addr) {
				seq_printf(s, "Failed local validate memory: value 0x%x, "
						   "*addr 0x%x, i %i\n", value, *addr, i);
				break;
			}
			++addr;
		}
		seq_printf(s, "Validate local memory: value 0x%x\n", value);
	}

	if (*test_remote_dma_addr) {
		/* Transfer DMA to other side */
		void *remapped = NULL;
		unsigned long src_pages = 0;
		dma_addr_t src_pa = 0;
		dma_cookie_t cookie;
		dma_addr_t dst = 0;
		char value;
		DECLARE_WAIT_QUEUE_HEAD_ONSTACK(done_wait);
		struct dmatest_done done = { .wait = &done_wait };

		seq_printf(s, "DMA Transfer to remote side *test_remote_dma_addr 0x%llx\n",
				   *test_remote_dma_addr);

		src_pages = __get_free_pages(GFP_KERNEL | __GFP_ZERO | __GFP_DMA32,
					   get_order(vop_dma_test_size));
		if (!src_pages) {
			seq_printf(s, "failed to allocate src buffer, size %lu\n",
					vop_dma_test_size);
			goto err_dma;
		}
		src_pa = dma_map_single(&vpdev->dev, (void*)src_pages,
				vop_dma_test_size, DMA_FROM_DEVICE);
		ret = dma_mapping_error(&vpdev->dev, src_pa);
		if (ret) {
			seq_printf(s, "failed to dma map buffer: %i\n", ret);
			goto err_dma;
		}

		remapped = vpdev->hw_ops->ioremap(vpdev, (u64)*test_remote_dma_addr,
				vop_dma_test_size);

		if (!remapped) {
			seq_printf(s, "Error remap memory\n");
			goto err_dma;
		}

		dst = (u64)vpdev->aper->pa + (remapped - vpdev->aper->va);

		seq_printf(s, "remapped: 0x%p dst: 0x%llx aper->pa: 0x%llx "
				   "aper->va: 0x%p\n", remapped, dst,
				   vpdev->aper->pa, vpdev->aper->va);

		value = ioread8(remapped);
		++value;
		seq_printf(s, "Write remote memory: value 0x%x\n", value);
		memset(phys_to_virt(src_pa), value, vop_dma_test_size);

		cookie = vop_dma_async(vpdev, dst, src_pa, vop_dma_test_size,
				dmatest_callback, (void *)&done);

		if (dma_submit_error(cookie)) {
			seq_printf(s, "DMA Transfer schedule error %i\n", cookie);
			goto err_dma;
		}

		if (wait_event_interruptible_timeout(done_wait, done.done,
				     msecs_to_jiffies(2000)) < 0 || !done.done) {
			seq_printf(s, "DMA Transfer FAILED no callback %i\n", cookie);
			goto err_dma;
		} else {
			seq_printf(s, "DMA Transfer DONE \n");
		}

	err_dma:
		if(src_pa) {
			dma_unmap_single(&vpdev->dev, src_pa, vop_dma_test_size,
					DMA_FROM_DEVICE);
			src_pa = 0;
		}
		if (src_pages) {
			free_pages(src_pages, get_order(vop_dma_test_size));
			src_pages = 0;
		}
		if (remapped) {
			vpdev->hw_ops->iounmap(vpdev, remapped);
			remapped = NULL;
		}
	} else {
		seq_printf(s, "To test DMA transfer first run this test on the "
				   "other side.\n");
	}

end:
	return 0;
}

static void vop_dma_test_clean(struct vop_info *vi)
{
	struct vop_device *vpdev = vi->vpdev;
	bool link_side = vpdev->hw_ops->is_link_side(vpdev);
	struct vca_bootparam *bootparam = vpdev->hw_ops->get_dp(vpdev);
	__u64 *test_local_dma_addr;

	if (link_side) {
		test_local_dma_addr = &bootparam->test_card_dma_addr;
	} else {
		test_local_dma_addr = &bootparam->test_host_dma_addr;
	}

	if (*test_local_dma_addr) {
		dma_unmap_single(&vpdev->dev, *test_local_dma_addr, vop_dma_test_size,
				DMA_BIDIRECTIONAL);
		*test_local_dma_addr = 0;
	}

	if (vi->dbg.dma_test_pages) {
		free_pages(vi->dbg.dma_test_pages, get_order(vop_dma_test_size));
		vi->dbg.dma_test_pages = 0;
	}
}

static int vop_dma_test_open(struct inode *inode, struct file *file)
{
	return single_open(file, vop_dma_test_show, inode->i_private);
}

static const struct file_operations dma_test_ops = {
	.owner   = THIS_MODULE,
	.open    = vop_dma_test_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

void vop_init_debugfs(struct vop_info *vi)
{
	char name[16];

	snprintf(name, sizeof(name), "%s%d", KBUILD_MODNAME, vi->vpdev->dnode);
	vi->dbg.debug_fs = debugfs_create_dir(name, NULL);
	if (!vi->dbg.debug_fs) {
		pr_err("can't create debugfs dir vop name %s\n", name);
		return;
	}
	debugfs_create_file("dp", 0444, vi->dbg.debug_fs, vi, &dp_ops);
	debugfs_create_file("vdev_info", 0444, vi->dbg.debug_fs, vi, &vdev_info_ops);
	debugfs_create_file("crash", 0444, vi->dbg.debug_fs, vi, &crash_ops);
	debugfs_create_file("panic", 0444, vi->dbg.debug_fs, vi, &panic_ops);
	debugfs_create_file("stats", 0444, vi->dbg.debug_fs, vi, &stats_ops);
	debugfs_create_file("dma_test", 0444, vi->dbg.debug_fs, vi, &dma_test_ops);
}

void vop_exit_debugfs(struct vop_info *vi)
{
	vop_dma_test_clean(vi);
	debugfs_remove_recursive(vi->dbg.debug_fs);
}
