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
#include <asm/tlbflush.h>
#include <asm/tlb.h>

/* Not use in release version, from performance reasons. */
//#define DEBUG_ALM_CHECK

#include "vca_dev_common.h"
#include "vca_common.h"
#include "plx_device.h"
#include "plx_hw.h"
#include "plx_alm.h"
#include "plx_lbp.h"


/*
 * Global variables are declared as static, so are global within the file.
 */


extern struct plx_device* vca_get_xdev_pointer(int card, int cpu);
static struct plx_device *somnath_xdev = NULL; 		

#define MAX_MAPS_NR 2 
static struct kobject *sgx5_kobject;

struct sgx5_map_struct {
        unsigned long remote_phys_addr;
        unsigned long len;
        unsigned long local_phys_addr;
};
	
static struct sgx5_map_struct stored_maps[MAX_MAPS_NR] = {{0,},};


static int sgx5_map(unsigned long remote_dma_phys_address, unsigned long size, unsigned long *paddr)
{

        unsigned long *remote_access_address = NULL;
        if (somnath_xdev) {
                remote_access_address= plx_ioremap(somnath_xdev, (dma_addr_t)remote_dma_phys_address, size);
                if (remote_access_address) {
                        *paddr = somnath_xdev->aper.pa + ((void *)remote_access_address - somnath_xdev->aper.va);
                        printk("somnath: mapped physical address 0x%lx in plx\n",*paddr);
                }
                else  {
                        printk("somnath: error mapping size %ld bytes in plx device\n",size);
			*paddr = 0;
                        return -ENODEV;
                }
        }

//	*paddr = remote_dma_phys_address+size;
        return 0;

}

static void sgx5_unmap(unsigned long *paddr)
{

        unsigned long *remote_access_address;
        if (somnath_xdev) {
                remote_access_address = (*paddr - somnath_xdev->aper.pa) + somnath_xdev->aper.va;
                plx_iounmap(somnath_xdev,remote_access_address);
                printk("somnath: unmapped physical address is 0x%lx from plx\n",*paddr);
		*paddr = 0;
        }

//	*paddr = 0;

}


static ssize_t read_map0(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
	return sprintf(buf,"0x%lx 0x%lx 0x%lx\n",stored_maps[0].remote_phys_addr,stored_maps[0].len,stored_maps[0].local_phys_addr);
}


static ssize_t write_map0(struct kobject *kobj, struct kobj_attribute *attr,
                     const char *buf, size_t count)
{
        sscanf(buf, "0x%lx 0x%lx", &stored_maps[0].remote_phys_addr, &stored_maps[0].len);
	printk("somnath: phys_addr=0x%lx len=0x%lx\n",stored_maps[0].remote_phys_addr,stored_maps[0].len);

	if ((stored_maps[0].remote_phys_addr == 0) || (stored_maps[0].len == 0)) {
		if (stored_maps[0].local_phys_addr != 0) {
			sgx5_unmap(&stored_maps[0].local_phys_addr);	
			printk("somnath: unmap success remote_phys_addr=0x%lx len=0x%lx\n",stored_maps[0].remote_phys_addr,stored_maps[0].len);
		}
		stored_maps[0].len = 0;
	 	stored_maps[0].remote_phys_addr = 0;
	} else {
	 	if (sgx5_map(stored_maps[0].remote_phys_addr,stored_maps[0].len,&stored_maps[0].local_phys_addr) != 0) {
			printk("somnath: mapping fail remote_phys_addr=0x%lx len=0x%lx\n",stored_maps[0].remote_phys_addr,stored_maps[0].len);
			stored_maps[0].remote_phys_addr = 0;
			stored_maps[0].len = 0;
			return -1;
		} else {
			printk("somnath: mapping succs remote_phys_addr=0x%lx len=0x%lx\n",stored_maps[0].remote_phys_addr,stored_maps[0].len);
	        }
	}

        return count;
}

static ssize_t read_map1(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
        return sprintf(buf,"0x%lx 0x%lx 0x%lx\n",stored_maps[1].remote_phys_addr,stored_maps[1].len,stored_maps[1].local_phys_addr);
}


static ssize_t write_map1(struct kobject *kobj, struct kobj_attribute *attr,
                     const char *buf, size_t count)
{
        sscanf(buf, "0x%lx 0x%lx", &stored_maps[1].remote_phys_addr, &stored_maps[1].len);
        printk("somnath: phys_addr=0x%lx len=0x%lx\n",stored_maps[1].remote_phys_addr,stored_maps[1].len);

        if ((stored_maps[1].remote_phys_addr == 0) || (stored_maps[1].len == 0)) {
                if (stored_maps[1].local_phys_addr != 0) {
                        sgx5_unmap(&stored_maps[1].local_phys_addr);
                        printk("somnath: unmap success remote_phys_addr=0x%lx len=0x%lx\n",stored_maps[1].remote_phys_addr,stored_maps[1].len);
                }
                stored_maps[1].len = 0;
                stored_maps[1].remote_phys_addr = 0;
        } else {
                if (sgx5_map(stored_maps[1].remote_phys_addr,stored_maps[1].len,&stored_maps[1].local_phys_addr) != 0) {
                        printk("somnath: mapping fail remote_phys_addr=0x%lx len=0x%lx\n",stored_maps[1].remote_phys_addr,stored_maps[1].len);
                        stored_maps[0].remote_phys_addr = 0;
                        stored_maps[0].len = 0;
                        return -1;
                } else {
                        printk("somnath: mapping succs remote_phys_addr=0x%lx len=0x%lx\n",stored_maps[1].remote_phys_addr,stored_maps[1].len);
                }
        }

        return count;
}


static struct kobj_attribute map0_attribute =__ATTR(map0, 0660, read_map0, write_map0);
static struct kobj_attribute map1_attribute =__ATTR(map1, 0660, read_map1, write_map1);

static int __init sgx5_02_init (void)
{
        int error = 0;

	somnath_xdev = (struct plx_device *)vca_get_xdev_pointer(0,2); 		
	//somnath_xdev = (struct plx_device *)0xffff88189a38a800ULL; 		
//	printk("somnath: vca get xdev pointer %p\n",vca_get_xdev_pointer);
	printk("somnath 0 2 : xdev %p aper.va %p aper.pa 0x%lx\n",somnath_xdev, somnath_xdev->aper.va, somnath_xdev->aper.pa);

        sgx5_kobject = kobject_create_and_add("sgx5_mapper_2",kernel_kobj);
        if(!sgx5_kobject)
                return -ENOMEM;

        error = sysfs_create_file(sgx5_kobject, &map0_attribute.attr);
        if (error) {
                pr_debug("somnath: failed to create the map 0 file in /sys/kernel/sgx5_mapper_02 \n");
        }


        error = sysfs_create_file(sgx5_kobject, &map1_attribute.attr);
        if (error) {
                pr_debug("somnath: failed to create the map 1 file in /sys/kernel/sgx5_mapper_02 \n");
        }

        return error;
}

static void __exit sgx5_02_exit (void)
{
        pr_debug ("somnath: Module 02 un initialized successfully \n");
        kobject_put(sgx5_kobject);
}


module_init(sgx5_02_init);
module_exit(sgx5_02_exit);
MODULE_LICENSE("GPL");
