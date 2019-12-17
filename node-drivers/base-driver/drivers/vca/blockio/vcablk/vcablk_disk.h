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
 * Adapted from:
 *
 * sbull examples, the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 *
 * Intel VCA Block IO driver.
 *
 */
#ifndef __VCA_BLK_DISK_H__
#define __VCA_BLK_DISK_H__
#include <linux/bio.h>
#include "../vcablk_common/vcablk_common.h"

/* Minimal queue sizes */
/* #define VCABLK_BIO_CTX_POOL_SIZE		1
#define VCABLK_QUEUE_ACK_NUMS		1
#define VCABLK_BVEC_MAP_POOL_SIZE	256
#define VCABLK_BIO_CONTEXT_POOL_SIZE	256 */

#define VCABLK_BIO_CTX_POOL_SIZE		128
#define VCABLK_QUEUE_ACK_NUMS		(VCABLK_BIO_CTX_POOL_SIZE)
#define VCABLK_BVEC_MAP_POOL_SIZE	(VCABLK_BIO_CTX_POOL_SIZE * 4)
#define VCABLK_BIO_CONTEXT_POOL_SIZE	VCABLK_BVEC_MAP_POOL_SIZE

/* is a constant expression is not a power of 2 */
#define CHECK_NOT_POWER_OF_2(n) ((n) == 0 || (((n) & ((n) - 1)) != 0))

#if CHECK_NOT_POWER_OF_2(VCABLK_BIO_CONTEXT_POOL_SIZE) || \
	CHECK_NOT_POWER_OF_2(VCABLK_QUEUE_ACK_NUMS)
#error "RING SIZE HAVE TO BE POWER OF 2"
#endif

#if VCABLK_QUEUE_ACK_NUMS < VCABLK_BIO_CTX_POOL_SIZE
#error "Backend response can be blocked by too small ack queue"
#endif

#if VCABLK_BIO_CONTEXT_POOL_SIZE < VCABLK_BVEC_MAP_POOL_SIZE
#error "Backend request queue is too short, can block request"
#endif

#if VCABLK_BVEC_MAP_POOL_SIZE < BIO_MAX_PAGES
#error "Too small size of mapping list min is 256"
#endif

struct vcablk_disk;
struct vcablk_dev;

int vcablk_disk_init(struct vcablk_dev* bd);
void vcablk_disk_exit(void);

struct vcablk_disk* vcablk_disk_create(struct vcablk_dev* fdev, int uniq_id, size_t size,
			bool read_only, struct vcablk_ring *ring_req, struct vcablk_ring *completion_ring,
			int done_db_local, int request_db);
int vcablk_disk_start(struct vcablk_disk *dev);

int vcablk_disk_stop(struct vcablk_disk *dev, bool force);
int vcablk_disk_destroy(struct vcablk_disk *dev);
struct vcablk_ring * vcablk_disk_get_rings_req(struct vcablk_disk *dev);
struct vcablk_ring * vcablk_disk_get_rings_ack(struct vcablk_disk *dev);

#endif /* __VCA_BLK_DISK_H__ */
