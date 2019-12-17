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
	{ 0x3356b90b, __VMLINUX_SYMBOL_STR(cpu_tss) },
	{ 0x50c7ed6d, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x81348cf2, __VMLINUX_SYMBOL_STR(node_to_cpumask_map) },
	{ 0x5349e8ad, __VMLINUX_SYMBOL_STR(debugfs_create_dir) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0x205c430c, __VMLINUX_SYMBOL_STR(single_open) },
	{ 0x754d539c, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x92a9c60c, __VMLINUX_SYMBOL_STR(time_to_tm) },
	{ 0x7e44da37, __VMLINUX_SYMBOL_STR(single_release) },
	{ 0x5624fead, __VMLINUX_SYMBOL_STR(seq_puts) },
	{ 0xc181f6c1, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0x56d4ca56, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0x793f4592, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0xaf81f3a0, __VMLINUX_SYMBOL_STR(filp_close) },
	{ 0xa8058269, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0xb42dab5, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x21e992a5, __VMLINUX_SYMBOL_STR(ida_simple_get) },
	{ 0xf2abd055, __VMLINUX_SYMBOL_STR(debugfs_create_file) },
	{ 0x12fbcff, __VMLINUX_SYMBOL_STR(vcablkebe_register) },
	{ 0x1a717561, __VMLINUX_SYMBOL_STR(debugfs_remove_recursive) },
	{ 0x5ffda2b8, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0xf1e1a95b, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x343a1a8, __VMLINUX_SYMBOL_STR(__list_add) },
	{ 0xb9521d26, __VMLINUX_SYMBOL_STR(vop_unregister_device) },
	{ 0x8a0b12c6, __VMLINUX_SYMBOL_STR(complete_all) },
	{ 0xf6f1b97b, __VMLINUX_SYMBOL_STR(vcablkbe_unregister_f2b_callback) },
	{ 0x411a44dd, __VMLINUX_SYMBOL_STR(vcablk_unregister) },
	{ 0x9e88526, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xdcbe92b0, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0xb5be1209, __VMLINUX_SYMBOL_STR(vca_csm_unregister_device) },
	{ 0xfb578fc5, __VMLINUX_SYMBOL_STR(memset) },
	{ 0xf10de535, __VMLINUX_SYMBOL_STR(ioread8) },
	{ 0x4d22d4b4, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x1916e38c, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x37befc70, __VMLINUX_SYMBOL_STR(jiffies_to_msecs) },
	{ 0x3c8bb534, __VMLINUX_SYMBOL_STR(vca_csm_register_device) },
	{ 0x738b23e9, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x8b78dfe4, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x27882b9b, __VMLINUX_SYMBOL_STR(ida_simple_remove) },
	{ 0x2e2eef02, __VMLINUX_SYMBOL_STR(dma_sync_wait) },
	{ 0x9166fada, __VMLINUX_SYMBOL_STR(strncpy) },
	{ 0x626d5e74, __VMLINUX_SYMBOL_STR(vop_stop_network_traffic) },
	{ 0xeb9f957e, __VMLINUX_SYMBOL_STR(debugfs_remove) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0x89032054, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0x521445b, __VMLINUX_SYMBOL_STR(list_del) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x3df98a89, __VMLINUX_SYMBOL_STR(sysfs_remove_link) },
	{ 0x952664c5, __VMLINUX_SYMBOL_STR(do_exit) },
	{ 0xfe5d4bb2, __VMLINUX_SYMBOL_STR(sys_tz) },
	{ 0xdd9edeaa, __VMLINUX_SYMBOL_STR(arch_dma_alloc_attrs) },
	{ 0x41f7cc79, __VMLINUX_SYMBOL_STR(sysfs_create_link) },
	{ 0xd56e3616, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0x78764f4e, __VMLINUX_SYMBOL_STR(pv_irq_ops) },
	{ 0x23868740, __VMLINUX_SYMBOL_STR(vcablk_register) },
	{ 0xc05a3af9, __VMLINUX_SYMBOL_STR(vca_mgr_unregister_device) },
	{ 0x42c8de35, __VMLINUX_SYMBOL_STR(ioremap_nocache) },
	{ 0xa39c86e1, __VMLINUX_SYMBOL_STR(vca_mgr_extd_register_device) },
	{ 0xa916b694, __VMLINUX_SYMBOL_STR(strnlen) },
	{ 0x6edd721c, __VMLINUX_SYMBOL_STR(vca_mgr_register_device) },
	{ 0x12a38747, __VMLINUX_SYMBOL_STR(usleep_range) },
	{ 0x913c83d5, __VMLINUX_SYMBOL_STR(pci_bus_read_config_dword) },
	{ 0x93fca811, __VMLINUX_SYMBOL_STR(__get_free_pages) },
	{ 0x43592b0, __VMLINUX_SYMBOL_STR(pci_intx) },
	{ 0x727c4f3, __VMLINUX_SYMBOL_STR(iowrite8) },
	{ 0xd97954bf, __VMLINUX_SYMBOL_STR(vca_csa_register_device) },
	{ 0x878cd015, __VMLINUX_SYMBOL_STR(wait_for_completion_interruptible_timeout) },
	{ 0x1ee9adbc, __VMLINUX_SYMBOL_STR(vcablkbe_register_f2b_callback) },
	{ 0x2ec56424, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0x1ef9b6d8, __VMLINUX_SYMBOL_STR(vop_register_device) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x2e9db0d5, __VMLINUX_SYMBOL_STR(__dma_request_channel) },
	{ 0xe0fa1a6f, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0xc527372c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0xbe361843, __VMLINUX_SYMBOL_STR(pci_ioremap_bar) },
	{ 0xe81173f4, __VMLINUX_SYMBOL_STR(__dynamic_dev_dbg) },
	{ 0x680ec266, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0x4302d0eb, __VMLINUX_SYMBOL_STR(free_pages) },
	{ 0xa6bbd805, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x79eae55e, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x4f68e5c9, __VMLINUX_SYMBOL_STR(do_gettimeofday) },
	{ 0x3365edb0, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0xd8b61f7e, __VMLINUX_SYMBOL_STR(dma_release_channel) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x2baefd6b, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0xac50100e, __VMLINUX_SYMBOL_STR(pci_disable_msi) },
	{ 0xd4c7a2dc, __VMLINUX_SYMBOL_STR(dma_supported) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x6aac2e17, __VMLINUX_SYMBOL_STR(vcablkebe_unregister) },
	{ 0xadff4bfc, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xc1641ae7, __VMLINUX_SYMBOL_STR(pci_get_device) },
	{ 0xab9f76ba, __VMLINUX_SYMBOL_STR(dev_warn) },
	{ 0xbcb94251, __VMLINUX_SYMBOL_STR(vca_csa_unregister_device) },
	{ 0x7f02188f, __VMLINUX_SYMBOL_STR(__msecs_to_jiffies) },
	{ 0x30b04526, __VMLINUX_SYMBOL_STR(ida_init) },
	{ 0x436c2179, __VMLINUX_SYMBOL_STR(iowrite32) },
	{ 0xba85cd7, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0x263074f8, __VMLINUX_SYMBOL_STR(irq_set_affinity_hint) },
	{ 0xb5419b40, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0xacb99769, __VMLINUX_SYMBOL_STR(ida_destroy) },
	{ 0xf9c9ca9, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0xe484e35f, __VMLINUX_SYMBOL_STR(ioread32) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xf9bba10b, __VMLINUX_SYMBOL_STR(filp_open) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=vcablk_bckend,vop_bus,vcablkfe,vca_csm_bus,vop,vca_mgr_bus,vca_mgr_extd_bus,vca_csa_bus";

MODULE_ALIAS("pci:v000010B5d000087A0sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d000087A1sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d000087B0sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d000087B1sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00002953sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00002958sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00002959sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d0000295Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00002954sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00002955sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00002956sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "14AF746D32A04BB5ABA4827");
