#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0x28950ef1, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x98ab5c8d, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0xfd96706c, __VMLINUX_SYMBOL_STR(node_to_cpumask_map) },
	{ 0xda3e43d1, __VMLINUX_SYMBOL_STR(_raw_spin_unlock) },
	{ 0x9f13414d, __VMLINUX_SYMBOL_STR(debugfs_create_dir) },
	{ 0x1e0c2be4, __VMLINUX_SYMBOL_STR(ioremap_wc) },
	{ 0x16a5a12f, __VMLINUX_SYMBOL_STR(single_open) },
	{ 0x754d539c, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0xc483a55a, __VMLINUX_SYMBOL_STR(dev_set_drvdata) },
	{ 0x92a9c60c, __VMLINUX_SYMBOL_STR(time_to_tm) },
	{ 0x59d5a7f7, __VMLINUX_SYMBOL_STR(dma_set_mask) },
	{ 0x2296f507, __VMLINUX_SYMBOL_STR(single_release) },
	{ 0x50612798, __VMLINUX_SYMBOL_STR(seq_puts) },
	{ 0x11ab7446, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0xd566fe45, __VMLINUX_SYMBOL_STR(pci_pcie_type) },
	{ 0x74df1d4, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0xa16aae11, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x5b8adbca, __VMLINUX_SYMBOL_STR(x86_dma_fallback_dev) },
	{ 0xcbd67ddb, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0x4ed12f73, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x44e6ecc8, __VMLINUX_SYMBOL_STR(ida_simple_get) },
	{ 0xae9bb4d4, __VMLINUX_SYMBOL_STR(debugfs_create_file) },
	{ 0x6ce07041, __VMLINUX_SYMBOL_STR(vcablkebe_register) },
	{ 0x636c619f, __VMLINUX_SYMBOL_STR(debugfs_remove_recursive) },
	{ 0x9c3df9b4, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0xc35e4b4e, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x343a1a8, __VMLINUX_SYMBOL_STR(__list_add) },
	{ 0xbbbd61aa, __VMLINUX_SYMBOL_STR(vop_unregister_device) },
	{ 0x40256835, __VMLINUX_SYMBOL_STR(complete_all) },
	{ 0xe7a3687e, __VMLINUX_SYMBOL_STR(vcablkbe_unregister_f2b_callback) },
	{ 0x7e75229a, __VMLINUX_SYMBOL_STR(vcablk_unregister) },
	{ 0xf432dd3d, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xf33202f, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0x4e828b45, __VMLINUX_SYMBOL_STR(vca_csm_unregister_device) },
	{ 0xfb578fc5, __VMLINUX_SYMBOL_STR(memset) },
	{ 0xf10de535, __VMLINUX_SYMBOL_STR(ioread8) },
	{ 0xd795224, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x8f64aa4, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x37befc70, __VMLINUX_SYMBOL_STR(jiffies_to_msecs) },
	{ 0x70b380f2, __VMLINUX_SYMBOL_STR(vca_csm_register_device) },
	{ 0x9a025cd5, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x42f90a31, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0xa9d8f55e, __VMLINUX_SYMBOL_STR(ida_simple_remove) },
	{ 0x23950edc, __VMLINUX_SYMBOL_STR(dma_sync_wait) },
	{ 0x9166fada, __VMLINUX_SYMBOL_STR(strncpy) },
	{ 0xa64c137d, __VMLINUX_SYMBOL_STR(vop_stop_network_traffic) },
	{ 0x4c48a854, __VMLINUX_SYMBOL_STR(debugfs_remove) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0x9abdea30, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0x521445b, __VMLINUX_SYMBOL_STR(list_del) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x5d6346c9, __VMLINUX_SYMBOL_STR(sysfs_remove_link) },
	{ 0x952664c5, __VMLINUX_SYMBOL_STR(do_exit) },
	{ 0xfe5d4bb2, __VMLINUX_SYMBOL_STR(sys_tz) },
	{ 0x533a4987, __VMLINUX_SYMBOL_STR(sysfs_create_link) },
	{ 0x4cce8b07, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0x78764f4e, __VMLINUX_SYMBOL_STR(pv_irq_ops) },
	{ 0xac9ed95d, __VMLINUX_SYMBOL_STR(vcablk_register) },
	{ 0x60659fa, __VMLINUX_SYMBOL_STR(vca_mgr_unregister_device) },
	{ 0xae7f0d80, __VMLINUX_SYMBOL_STR(vca_mgr_extd_register_device) },
	{ 0xa916b694, __VMLINUX_SYMBOL_STR(strnlen) },
	{ 0xc9cc1bbb, __VMLINUX_SYMBOL_STR(vca_mgr_register_device) },
	{ 0x12a38747, __VMLINUX_SYMBOL_STR(usleep_range) },
	{ 0x93fca811, __VMLINUX_SYMBOL_STR(__get_free_pages) },
	{ 0xe3fd3e7d, __VMLINUX_SYMBOL_STR(pci_intx) },
	{ 0x3bd1b1f6, __VMLINUX_SYMBOL_STR(msecs_to_jiffies) },
	{ 0x727c4f3, __VMLINUX_SYMBOL_STR(iowrite8) },
	{ 0x66cf7831, __VMLINUX_SYMBOL_STR(vca_csa_register_device) },
	{ 0x263122f8, __VMLINUX_SYMBOL_STR(wait_for_completion_interruptible_timeout) },
	{ 0x7241a6fb, __VMLINUX_SYMBOL_STR(pci_read_config_dword) },
	{ 0xb5b39452, __VMLINUX_SYMBOL_STR(vcablkbe_register_f2b_callback) },
	{ 0xe65cdceb, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xee8884db, __VMLINUX_SYMBOL_STR(vop_register_device) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x4d5a9086, __VMLINUX_SYMBOL_STR(__dma_request_channel) },
	{ 0x5be2a1b5, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0x41ec4c1a, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xd52bf1ce, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x14d9671b, __VMLINUX_SYMBOL_STR(pci_ioremap_bar) },
	{ 0xc140ad72, __VMLINUX_SYMBOL_STR(__dynamic_dev_dbg) },
	{ 0x9327f5ce, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0x4302d0eb, __VMLINUX_SYMBOL_STR(free_pages) },
	{ 0xcf21d241, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x8c34c149, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x4f68e5c9, __VMLINUX_SYMBOL_STR(do_gettimeofday) },
	{ 0x1685c91c, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0xf352393, __VMLINUX_SYMBOL_STR(dma_release_channel) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xd51fb019, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0xa0962794, __VMLINUX_SYMBOL_STR(pci_disable_msi) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0xe52712bc, __VMLINUX_SYMBOL_STR(vcablkebe_unregister) },
	{ 0x45140522, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xb0b14c08, __VMLINUX_SYMBOL_STR(pci_get_device) },
	{ 0x43b38448, __VMLINUX_SYMBOL_STR(dev_warn) },
	{ 0xa202b225, __VMLINUX_SYMBOL_STR(vca_csa_unregister_device) },
	{ 0x5e15d54c, __VMLINUX_SYMBOL_STR(ida_init) },
	{ 0x436c2179, __VMLINUX_SYMBOL_STR(iowrite32) },
	{ 0x6cc1bfad, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0xaa011360, __VMLINUX_SYMBOL_STR(irq_set_affinity_hint) },
	{ 0x77e2f33, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x82477240, __VMLINUX_SYMBOL_STR(ida_destroy) },
	{ 0x7cf5b2b3, __VMLINUX_SYMBOL_STR(dev_get_drvdata) },
	{ 0x584c5b17, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0xe484e35f, __VMLINUX_SYMBOL_STR(ioread32) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
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

MODULE_INFO(srcversion, "25CD29E90E24E8BBACAFF87");
MODULE_INFO(rhelversion, "7.3");
