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
	{ 0x5744755c, __VMLINUX_SYMBOL_STR(device_remove_file) },
	{ 0x50c7ed6d, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x81348cf2, __VMLINUX_SYMBOL_STR(node_to_cpumask_map) },
	{ 0x619cb7dd, __VMLINUX_SYMBOL_STR(simple_read_from_buffer) },
	{ 0x5349e8ad, __VMLINUX_SYMBOL_STR(debugfs_create_dir) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0x28acb511, __VMLINUX_SYMBOL_STR(pcim_enable_device) },
	{ 0x205c430c, __VMLINUX_SYMBOL_STR(single_open) },
	{ 0x248272be, __VMLINUX_SYMBOL_STR(pcim_iomap_table) },
	{ 0x7e44da37, __VMLINUX_SYMBOL_STR(single_release) },
	{ 0x5624fead, __VMLINUX_SYMBOL_STR(seq_puts) },
	{ 0x56d4ca56, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0x565a2c20, __VMLINUX_SYMBOL_STR(dma_async_tx_descriptor_init) },
	{ 0xeae3dfd6, __VMLINUX_SYMBOL_STR(__const_udelay) },
	{ 0x999e8297, __VMLINUX_SYMBOL_STR(vfree) },
	{ 0xf2abd055, __VMLINUX_SYMBOL_STR(debugfs_create_file) },
	{ 0x1a717561, __VMLINUX_SYMBOL_STR(debugfs_remove_recursive) },
	{ 0x5ffda2b8, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0x343a1a8, __VMLINUX_SYMBOL_STR(__list_add) },
	{ 0x4add1203, __VMLINUX_SYMBOL_STR(dma_async_device_register) },
	{ 0xdcbe92b0, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0xfb578fc5, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x4d22d4b4, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xa1c76e0a, __VMLINUX_SYMBOL_STR(_cond_resched) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0xd8e76cd, __VMLINUX_SYMBOL_STR(pcim_iomap_regions) },
	{ 0x92d8c360, __VMLINUX_SYMBOL_STR(device_create_file) },
	{ 0xdd9edeaa, __VMLINUX_SYMBOL_STR(arch_dma_alloc_attrs) },
	{ 0xd56e3616, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0x40a9b349, __VMLINUX_SYMBOL_STR(vzalloc) },
	{ 0x78764f4e, __VMLINUX_SYMBOL_STR(pv_irq_ops) },
	{ 0x76002add, __VMLINUX_SYMBOL_STR(devm_free_irq) },
	{ 0x43592b0, __VMLINUX_SYMBOL_STR(pci_intx) },
	{ 0xd62c833f, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0xc0cd3b13, __VMLINUX_SYMBOL_STR(___ratelimit) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x431bb916, __VMLINUX_SYMBOL_STR(pci_enable_msi_range) },
	{ 0xe0fa1a6f, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0xc527372c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0xe81173f4, __VMLINUX_SYMBOL_STR(__dynamic_dev_dbg) },
	{ 0xa6bbd805, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x2207a57f, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x3365edb0, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xac50100e, __VMLINUX_SYMBOL_STR(pci_disable_msi) },
	{ 0xd4c7a2dc, __VMLINUX_SYMBOL_STR(dma_supported) },
	{ 0xadff4bfc, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xf08242c2, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x28318305, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0xee5e73b1, __VMLINUX_SYMBOL_STR(dma_async_device_unregister) },
	{ 0x436c2179, __VMLINUX_SYMBOL_STR(iowrite32) },
	{ 0x263074f8, __VMLINUX_SYMBOL_STR(irq_set_affinity_hint) },
	{ 0xb442685a, __VMLINUX_SYMBOL_STR(devm_request_threaded_irq) },
	{ 0xf9c9ca9, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0xe484e35f, __VMLINUX_SYMBOL_STR(ioread32) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v000010B5d000087D0sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d000087E0sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00002952sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "22B2AAFADE0A1C15D0FEE17");
