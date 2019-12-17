#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x5e53561, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x8487a2b6, __VMLINUX_SYMBOL_STR(flush_work) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0x50c7ed6d, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x81348cf2, __VMLINUX_SYMBOL_STR(node_to_cpumask_map) },
	{ 0x619cb7dd, __VMLINUX_SYMBOL_STR(simple_read_from_buffer) },
	{ 0x5349e8ad, __VMLINUX_SYMBOL_STR(debugfs_create_dir) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0x205c430c, __VMLINUX_SYMBOL_STR(single_open) },
	{ 0x7e44da37, __VMLINUX_SYMBOL_STR(single_release) },
	{ 0x5624fead, __VMLINUX_SYMBOL_STR(seq_puts) },
	{ 0x56d4ca56, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0x44b1d426, __VMLINUX_SYMBOL_STR(__dynamic_pr_debug) },
	{ 0xb42dab5, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0xf2abd055, __VMLINUX_SYMBOL_STR(debugfs_create_file) },
	{ 0x1a717561, __VMLINUX_SYMBOL_STR(debugfs_remove_recursive) },
	{ 0x5ffda2b8, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0xf1e1a95b, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0xfabcd197, __VMLINUX_SYMBOL_STR(vca_vringh_need_notify_kern) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x343a1a8, __VMLINUX_SYMBOL_STR(__list_add) },
	{ 0x194ee7a7, __VMLINUX_SYMBOL_STR(vca_register_virtio_device) },
	{ 0x8a0b12c6, __VMLINUX_SYMBOL_STR(complete_all) },
	{ 0x9e88526, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0x5451229e, __VMLINUX_SYMBOL_STR(misc_register) },
	{ 0xfb578fc5, __VMLINUX_SYMBOL_STR(memset) },
	{ 0xf10de535, __VMLINUX_SYMBOL_STR(ioread8) },
	{ 0x4d22d4b4, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x37befc70, __VMLINUX_SYMBOL_STR(jiffies_to_msecs) },
	{ 0x738b23e9, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x5fadad33, __VMLINUX_SYMBOL_STR(device_find_child) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xf4cd7478, __VMLINUX_SYMBOL_STR(vca_vring_new_virtqueue) },
	{ 0x7c1372e8, __VMLINUX_SYMBOL_STR(panic) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x7d96cea3, __VMLINUX_SYMBOL_STR(wait_for_completion_interruptible) },
	{ 0x2e2eef02, __VMLINUX_SYMBOL_STR(dma_sync_wait) },
	{ 0xa1c76e0a, __VMLINUX_SYMBOL_STR(_cond_resched) },
	{ 0xffc8b81, __VMLINUX_SYMBOL_STR(vop_unregister_driver) },
	{ 0xcf886cb2, __VMLINUX_SYMBOL_STR(set_cpus_allowed_ptr) },
	{ 0x89032054, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0x521445b, __VMLINUX_SYMBOL_STR(list_del) },
	{ 0xd0b937b6, __VMLINUX_SYMBOL_STR(vca_vring_interrupt) },
	{ 0x952664c5, __VMLINUX_SYMBOL_STR(do_exit) },
	{ 0xb5c97c28, __VMLINUX_SYMBOL_STR(vca_unregister_virtio_device) },
	{ 0x52e89909, __VMLINUX_SYMBOL_STR(vca_vringh_init_kern) },
	{ 0xd56e3616, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0x78764f4e, __VMLINUX_SYMBOL_STR(pv_irq_ops) },
	{ 0xaf8eb12f, __VMLINUX_SYMBOL_STR(vca_virtio_check_driver_offered_feature) },
	{ 0x12a38747, __VMLINUX_SYMBOL_STR(usleep_range) },
	{ 0xa86b1174, __VMLINUX_SYMBOL_STR(put_device) },
	{ 0x545abd3e, __VMLINUX_SYMBOL_STR(vca_vringh_getdesc_kern) },
	{ 0x93fca811, __VMLINUX_SYMBOL_STR(__get_free_pages) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xd62c833f, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0xa202a8e5, __VMLINUX_SYMBOL_STR(kmalloc_order_trace) },
	{ 0x727c4f3, __VMLINUX_SYMBOL_STR(iowrite8) },
	{ 0x878cd015, __VMLINUX_SYMBOL_STR(wait_for_completion_interruptible_timeout) },
	{ 0x2ec56424, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xf8de09c, __VMLINUX_SYMBOL_STR(vca_vring_transport_features) },
	{ 0xc527372c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0xe81173f4, __VMLINUX_SYMBOL_STR(__dynamic_dev_dbg) },
	{ 0x4302d0eb, __VMLINUX_SYMBOL_STR(free_pages) },
	{ 0xa6bbd805, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x2207a57f, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x8c183cbe, __VMLINUX_SYMBOL_STR(iowrite16) },
	{ 0x3365edb0, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0xf8a50ad7, __VMLINUX_SYMBOL_STR(vop_register_driver) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xa72a0f5b, __VMLINUX_SYMBOL_STR(nr_online_nodes) },
	{ 0x7a4497db, __VMLINUX_SYMBOL_STR(kzfree) },
	{ 0xc5534d64, __VMLINUX_SYMBOL_STR(ioread16) },
	{ 0xf08242c2, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0xab9f76ba, __VMLINUX_SYMBOL_STR(dev_warn) },
	{ 0x2e0d2f7f, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0xb2d5a552, __VMLINUX_SYMBOL_STR(complete) },
	{ 0x28318305, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0x436c2179, __VMLINUX_SYMBOL_STR(iowrite32) },
	{ 0x5541ea93, __VMLINUX_SYMBOL_STR(on_each_cpu) },
	{ 0x95f5bbb0, __VMLINUX_SYMBOL_STR(vca_vringh_complete_kern) },
	{ 0xb5419b40, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x59ab1f24, __VMLINUX_SYMBOL_STR(vca_vring_del_virtqueue) },
	{ 0x62a21c0a, __VMLINUX_SYMBOL_STR(misc_deregister) },
	{ 0xf9c9ca9, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0xf779fb35, __VMLINUX_SYMBOL_STR(vca_virtio_net_features) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=vca_vringh,vca_virtio,vca_virtio_ring,vop_bus,vca_virtio_net";


MODULE_INFO(srcversion, "A65F66E42D4D69E6F82EC13");
