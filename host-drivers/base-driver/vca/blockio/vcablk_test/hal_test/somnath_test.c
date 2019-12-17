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
 * Intel VCA Block IO driver
 */
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/miscdevice.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/kallsyms.h>

/* Not use in release version, from performance reasons. */
//#define DEBUG_ALM_CHECK

#include "../../../common/vca_dev_common.h"
#include "../../../common/vca_common.h"
#include "../../../plx87xx/plx_device.h"
#include "../../../plx87xx/plx_hw.h"
#include "../../../plx87xx/plx_alm.h"
#include "../../../plx87xx/plx_lbp.h"



extern struct plx_device* vca_get_xdev_pointer(int card, int cpu);
void *remote_access_address = NULL;
struct plx_device *somnath_xdev = NULL; 		

unsigned long size=4096*1024;
EXPORT_SYMBOL_GPL(remote_access_address);
static int __init somnath_vcablk_bcknd_init(void)
{
	unsigned long output=0;
	int i = 0;
	somnath_xdev = (struct plx_device *)vca_get_xdev_pointer(0,0); 		
	printk("vca get xdev pointer %p\n",vca_get_xdev_pointer);
	if (somnath_xdev) {
		remote_access_address= plx_ioremap(somnath_xdev, (dma_addr_t)0x4573ff000, size);
		if (remote_access_address) 
			printk("somnath: %p %p %p pa %lx\n",somnath_xdev, somnath_xdev->aper.va, remote_access_address, somnath_xdev->aper.pa + (remote_access_address - somnath_xdev->aper.va));
		else 
			printk("error mapping size %d pages in plx\n",size/4096);
	}	
	//for (i=0; i<32; i++) 
	//printk("somnath: %d : %lx \n",i, plx_context_pointer[i]);
	//printk("somnath: %p \n",ks);
	//ifif (ks)	
	//printk("somnath: 0x%lx \n",ks->value);
	if (remote_access_address) {  
	 	//memcpy_fromio(&output, remote_access_address, 8);	
	//	printk("somnath: remote data is 0x%lx\n",output);
		printk("somnath: start  direct remote data is 0x%lx\n",*((unsigned long *)remote_access_address));
	//	printk("somnath: end remote data is 0x%lx\n",*((unsigned long *)(remote_access_address + size - 8 + 1)));
	}
	return 0;
}

static void somnath_vcablk_bcknd_exit(void)
{
	if (remote_access_address && somnath_xdev) {
	//	*((unsigned long *)remote_access_address) = 0x0123456789abcdefUL;
		plx_iounmap(somnath_xdev,remote_access_address);
	}

	printk("somnath: exit \n");
}

module_init(somnath_vcablk_bcknd_init);
module_exit(somnath_vcablk_bcknd_exit);
MODULE_LICENSE("GPL");
