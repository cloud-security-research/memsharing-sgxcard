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
 * Intel Virtio Over PCIe (VOP) Bus driver.
 *
 * This implementation is very similar to the the virtio bus driver
 * implementation @ drivers/virtio/virtio.c
 */
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/idr.h>
#include <linux/dma-mapping.h>

#include "vop_bus.h"

static ssize_t device_show(struct device *d,
			   struct device_attribute *attr, char *buf)
{
	struct vop_device *dev = dev_to_vop(d);
	return snprintf(buf, PAGE_SIZE, "0x%04x\n", dev->id.device);
}
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 11, 0)
static DEVICE_ATTR_RO(device);
#endif

static ssize_t vendor_show(struct device *d,
			   struct device_attribute *attr, char *buf)
{
	struct vop_device *dev = dev_to_vop(d);
	return snprintf(buf, PAGE_SIZE, "0x%04x\n", dev->id.vendor);
}
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 11, 0)
static DEVICE_ATTR_RO(vendor);
#endif

static ssize_t modalias_show(struct device *d,
			     struct device_attribute *attr, char *buf)
{
	struct vop_device *dev = dev_to_vop(d);
	return snprintf(buf, PAGE_SIZE, "vop:d%08Xv%08X\n",
		       dev->id.device, dev->id.vendor);
}
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 11, 0)
static DEVICE_ATTR_RO(modalias);

static struct attribute *vop_dev_attrs[] = {
	&dev_attr_device.attr,
	&dev_attr_vendor.attr,
	&dev_attr_modalias.attr,
	NULL,
};
ATTRIBUTE_GROUPS(vop_dev);
#else
static struct device_attribute vop_dev_attrs[] = {
	__ATTR_RO(device),
	__ATTR_RO(vendor),
	__ATTR_RO(modalias),
	__ATTR_NULL
};
#endif

static inline int vop_id_match(const struct vop_device *dev,
				  const struct vop_device_id *id)
{
	if (id->device != dev->id.device && id->device != VOP_DEV_ANY_ID)
		return 0;

	return id->vendor == VOP_DEV_ANY_ID || id->vendor == dev->id.vendor;
}

/*
 * This looks through all the IDs a driver claims to support.  If any of them
 * match, we return 1 and the kernel will call vop_dev_probe().
 */
static int vop_dev_match(struct device *dv, struct device_driver *dr)
{
	unsigned int i;
	struct vop_device *dev = dev_to_vop(dv);
	const struct vop_device_id *ids;

	ids = drv_to_vop(dr)->id_table;
	for (i = 0; ids[i].device; i++)
		if (vop_id_match(dev, &ids[i]))
			return 1;
	return 0;
}

static int vop_uevent(struct device *dv, struct kobj_uevent_env *env)
{
	struct vop_device *dev = dev_to_vop(dv);

	return add_uevent_var(env, "MODALIAS=vop:d%08Xv%08X",
			      dev->id.device, dev->id.vendor);
}

static int vop_dev_probe(struct device *d)
{
	int err;
	struct vop_device *dev = dev_to_vop(d);
	struct vop_driver *drv = drv_to_vop(dev->dev.driver);

	err = drv->probe(dev);
	if (!err)
		if (drv->scan)
			drv->scan(dev);
	return err;
}

static int vop_dev_remove(struct device *d)
{
	struct vop_device *dev = dev_to_vop(d);
	struct vop_driver *drv = drv_to_vop(dev->dev.driver);

	drv->remove(dev);
	return 0;
}

static struct bus_type vop_bus = {
	.name  = "vop_bus",
	.match = vop_dev_match,
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 11, 0)
	.dev_groups = vop_dev_groups,
#else
	.dev_attrs = vop_dev_attrs,
#endif
	.uevent = vop_uevent,
	.probe = vop_dev_probe,
	.remove = vop_dev_remove,
};

int vop_register_driver(struct vop_driver *driver)
{
	driver->driver.bus = &vop_bus;
	return driver_register(&driver->driver);
}
EXPORT_SYMBOL_GPL(vop_register_driver);

void vop_unregister_driver(struct vop_driver *driver)
{
	driver_unregister(&driver->driver);
}
EXPORT_SYMBOL_GPL(vop_unregister_driver);

static void vop_release_dev(struct device *d)
{
	struct vop_device *vdev = dev_to_vop(d);
        kfree(vdev);
}

struct vop_device *
vop_register_device(struct device *pdev, int id,
		    struct dma_map_ops *dma_ops, struct vop_hw_ops *hw_ops,
		    u8 dnode, struct vca_mw *aper, struct dma_chan *chan)
{
	int ret;
	u8 card_id, cpu_id;
	struct vop_device *vdev;

	vdev = kzalloc(sizeof(*vdev), GFP_KERNEL);
	if (!vdev)
		return ERR_PTR(-ENOMEM);

	vdev->dev.parent = pdev;
	vdev->id.device = id;
	vdev->id.vendor = VOP_DEV_ANY_ID;
	vdev->dev.archdata.dma_ops = dma_ops;
	vdev->dev.dma_mask = &vdev->dev.coherent_dma_mask;
	dma_set_mask(&vdev->dev, DMA_BIT_MASK(64));
	vdev->dev.release = vop_release_dev;
	vdev->hw_ops = hw_ops;
	dev_set_drvdata(&vdev->dev, vdev);
	vdev->dev.bus = &vop_bus;
	vdev->dnode = dnode;
	vdev->aper = aper;
	vdev->dma_ch = chan;
	vdev->hw_ops->get_card_and_cpu_id(vdev, &card_id, &cpu_id);
	dev_set_name(&vdev->dev, "vop-dev%u%u", card_id, cpu_id);
	/*
	 * device_register() causes the bus infrastructure to look for a
	 * matching driver.
	 */
	ret = device_register(&vdev->dev);
	if (ret)
		goto free_vdev;
	return vdev;
free_vdev:
	kfree(vdev);
	return ERR_PTR(ret);
}
EXPORT_SYMBOL_GPL(vop_register_device);

void vop_unregister_device(struct vop_device *dev)
{
	device_unregister(&dev->dev);
}
EXPORT_SYMBOL_GPL(vop_unregister_device);

static int __init vop_init(void)
{
	return bus_register(&vop_bus);
}

static void __exit vop_exit(void)
{
	bus_unregister(&vop_bus);
}

core_initcall(vop_init);
module_exit(vop_exit);

MODULE_AUTHOR("Intel Corporation");
MODULE_DESCRIPTION("Intel(R) VOP Bus driver");
MODULE_LICENSE("GPL v2");
