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
	{ 0x8487a2b6, __VMLINUX_SYMBOL_STR(flush_work) },
	{ 0x434693dd, __VMLINUX_SYMBOL_STR(alloc_pages_current) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xc897c382, __VMLINUX_SYMBOL_STR(sg_init_table) },
	{ 0x7d104b1f, __VMLINUX_SYMBOL_STR(skb_to_sgvec) },
	{ 0x2489d8c5, __VMLINUX_SYMBOL_STR(vca_virtqueue_get_buf) },
	{ 0x15692c87, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0xd93737a0, __VMLINUX_SYMBOL_STR(napi_disable) },
	{ 0x930484aa, __VMLINUX_SYMBOL_STR(cpu_online_mask) },
	{ 0x79aa04a2, __VMLINUX_SYMBOL_STR(get_random_bytes) },
	{ 0xc7a4fbed, __VMLINUX_SYMBOL_STR(rtnl_lock) },
	{ 0xe6d7097b, __VMLINUX_SYMBOL_STR(netif_carrier_on) },
	{ 0xc0a3d105, __VMLINUX_SYMBOL_STR(find_next_bit) },
	{ 0x6b06fdce, __VMLINUX_SYMBOL_STR(delayed_work_timer_fn) },
	{ 0x3eff7be7, __VMLINUX_SYMBOL_STR(netif_carrier_off) },
	{ 0xf087137d, __VMLINUX_SYMBOL_STR(__dynamic_pr_debug) },
	{ 0x4389d0ea, __VMLINUX_SYMBOL_STR(vca_virtqueue_poll) },
	{ 0x10eaf4a0, __VMLINUX_SYMBOL_STR(vca_virtqueue_add_inbuf) },
	{ 0x949f7342, __VMLINUX_SYMBOL_STR(__alloc_percpu) },
	{ 0xe1d1af76, __VMLINUX_SYMBOL_STR(__dev_kfree_skb_any) },
	{ 0x1976aa06, __VMLINUX_SYMBOL_STR(param_ops_bool) },
	{ 0x593a99b, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0x797c8fa9, __VMLINUX_SYMBOL_STR(cancel_delayed_work_sync) },
	{ 0xd87261b2, __VMLINUX_SYMBOL_STR(skb_partial_csum_set) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x81ad6f16, __VMLINUX_SYMBOL_STR(netif_napi_del) },
	{ 0xc9ec4e21, __VMLINUX_SYMBOL_STR(free_percpu) },
	{ 0x239b87a6, __VMLINUX_SYMBOL_STR(vca_virtqueue_disable_cb) },
	{ 0x2f592a90, __VMLINUX_SYMBOL_STR(skb_trim) },
	{ 0x2475ff29, __VMLINUX_SYMBOL_STR(__netdev_alloc_skb) },
	{ 0x99871637, __VMLINUX_SYMBOL_STR(vca_virtqueue_detach_unused_buf) },
	{ 0xfe7c4287, __VMLINUX_SYMBOL_STR(nr_cpu_ids) },
	{ 0xd795224, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x96cfe99d, __VMLINUX_SYMBOL_STR(vca_virtqueue_get_vring_size) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xd58bf9d8, __VMLINUX_SYMBOL_STR(ethtool_op_get_link) },
	{ 0xa7b8d77c, __VMLINUX_SYMBOL_STR(vca_unregister_virtio_driver) },
	{ 0xe27f9cc, __VMLINUX_SYMBOL_STR(vca_register_virtio_driver) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x708b5f0c, __VMLINUX_SYMBOL_STR(free_netdev) },
	{ 0xd7efe2ef, __VMLINUX_SYMBOL_STR(register_netdev) },
	{ 0x7be24743, __VMLINUX_SYMBOL_STR(vca_virtqueue_add_outbuf) },
	{ 0x3400f3b8, __VMLINUX_SYMBOL_STR(netif_receive_skb) },
	{ 0x5792f848, __VMLINUX_SYMBOL_STR(strlcpy) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0x8d62a52d, __VMLINUX_SYMBOL_STR(vca_virtqueue_enable_cb_prepare) },
	{ 0x2a62b5bc, __VMLINUX_SYMBOL_STR(netif_set_real_num_rx_queues) },
	{ 0x5562fbb2, __VMLINUX_SYMBOL_STR(netif_set_real_num_tx_queues) },
	{ 0x1f28e62f, __VMLINUX_SYMBOL_STR(vca_virtqueue_add_sgs) },
	{ 0x1fe9f800, __VMLINUX_SYMBOL_STR(unregister_cpu_notifier) },
	{ 0x3ff62317, __VMLINUX_SYMBOL_STR(local_bh_disable) },
	{ 0x423058c9, __VMLINUX_SYMBOL_STR(netif_device_attach) },
	{ 0xa281370e, __VMLINUX_SYMBOL_STR(__free_pages) },
	{ 0x71f122a5, __VMLINUX_SYMBOL_STR(vca_virtio_check_driver_offered_feature) },
	{ 0x2fa56500, __VMLINUX_SYMBOL_STR(netif_device_detach) },
	{ 0x670b9230, __VMLINUX_SYMBOL_STR(vca_virtqueue_kick) },
	{ 0x8b43159b, __VMLINUX_SYMBOL_STR(register_cpu_notifier) },
	{ 0x61a6b5a2, __VMLINUX_SYMBOL_STR(eth_prepare_mac_addr_change) },
	{ 0xd17f4c5b, __VMLINUX_SYMBOL_STR(__napi_schedule) },
	{ 0xeeec26a7, __VMLINUX_SYMBOL_STR(queue_delayed_work_on) },
	{ 0xc311ec22, __VMLINUX_SYMBOL_STR(cpu_possible_mask) },
	{ 0xd7d79132, __VMLINUX_SYMBOL_STR(put_online_cpus) },
	{ 0x195c9f2c, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0x799aca4, __VMLINUX_SYMBOL_STR(local_bh_enable) },
	{ 0x905307be, __VMLINUX_SYMBOL_STR(napi_complete_done) },
	{ 0xad480ea0, __VMLINUX_SYMBOL_STR(eth_type_trans) },
	{ 0xe1768727, __VMLINUX_SYMBOL_STR(netdev_notify_peers) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xc8341533, __VMLINUX_SYMBOL_STR(eth_commit_mac_addr_change) },
	{ 0x3efb35c9, __VMLINUX_SYMBOL_STR(get_online_cpus) },
	{ 0xd94cc09, __VMLINUX_SYMBOL_STR(__per_cpu_offset) },
	{ 0x384e70ae, __VMLINUX_SYMBOL_STR(__smp_mb__before_atomic) },
	{ 0x2a18c74, __VMLINUX_SYMBOL_STR(nf_conntrack_destroy) },
	{ 0x7efd609f, __VMLINUX_SYMBOL_STR(__netif_napi_add) },
	{ 0xf6ebc03b, __VMLINUX_SYMBOL_STR(net_ratelimit) },
	{ 0xdec0caf5, __VMLINUX_SYMBOL_STR(eth_validate_addr) },
	{ 0xb6244511, __VMLINUX_SYMBOL_STR(sg_init_one) },
	{ 0x4e4b4bd4, __VMLINUX_SYMBOL_STR(vca_virtqueue_is_broken) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x7628f3c7, __VMLINUX_SYMBOL_STR(this_cpu_off) },
	{ 0x4cbbd171, __VMLINUX_SYMBOL_STR(__bitmap_weight) },
	{ 0x43b38448, __VMLINUX_SYMBOL_STR(dev_warn) },
	{ 0x7eb952e6, __VMLINUX_SYMBOL_STR(unregister_netdev) },
	{ 0x2e0d2f7f, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0xc4bff15e, __VMLINUX_SYMBOL_STR(__netif_schedule) },
	{ 0x95baac39, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0x10e449d2, __VMLINUX_SYMBOL_STR(vca_virtqueue_enable_cb_delayed) },
	{ 0x2ac95217, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0x6e720ff2, __VMLINUX_SYMBOL_STR(rtnl_unlock) },
	{ 0x906ac89f, __VMLINUX_SYMBOL_STR(alloc_etherdev_mqs) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=vca_virtio_ring,vca_virtio";

MODULE_ALIAS("virtio:d00000001v*");

MODULE_INFO(srcversion, "2399D168E49D712B74C0884");
MODULE_INFO(rhelversion, "7.3");
