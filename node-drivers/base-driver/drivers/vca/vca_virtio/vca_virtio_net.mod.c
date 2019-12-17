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
	{ 0xdf4e5c93, __VMLINUX_SYMBOL_STR(alloc_pages_current) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xc897c382, __VMLINUX_SYMBOL_STR(sg_init_table) },
	{ 0x211c5d18, __VMLINUX_SYMBOL_STR(skb_to_sgvec) },
	{ 0xf66bf627, __VMLINUX_SYMBOL_STR(vca_virtqueue_get_buf) },
	{ 0x2a542879, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0xb1c63188, __VMLINUX_SYMBOL_STR(napi_disable) },
	{ 0x17795916, __VMLINUX_SYMBOL_STR(cpu_online_mask) },
	{ 0x79aa04a2, __VMLINUX_SYMBOL_STR(get_random_bytes) },
	{ 0xc7a4fbed, __VMLINUX_SYMBOL_STR(rtnl_lock) },
	{ 0xbb6bdb0f, __VMLINUX_SYMBOL_STR(netif_carrier_on) },
	{ 0xc0a3d105, __VMLINUX_SYMBOL_STR(find_next_bit) },
	{ 0x6b06fdce, __VMLINUX_SYMBOL_STR(delayed_work_timer_fn) },
	{ 0x9fc8b1c5, __VMLINUX_SYMBOL_STR(netif_carrier_off) },
	{ 0x44b1d426, __VMLINUX_SYMBOL_STR(__dynamic_pr_debug) },
	{ 0x1c319fd5, __VMLINUX_SYMBOL_STR(vca_virtqueue_poll) },
	{ 0x1dddd813, __VMLINUX_SYMBOL_STR(vca_virtqueue_add_inbuf) },
	{ 0x949f7342, __VMLINUX_SYMBOL_STR(__alloc_percpu) },
	{ 0xa0b19fba, __VMLINUX_SYMBOL_STR(__dev_kfree_skb_any) },
	{ 0x434c4819, __VMLINUX_SYMBOL_STR(param_ops_bool) },
	{ 0x9580deb, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0xa57863e, __VMLINUX_SYMBOL_STR(cancel_delayed_work_sync) },
	{ 0x4629334c, __VMLINUX_SYMBOL_STR(__preempt_count) },
	{ 0x67e870d0, __VMLINUX_SYMBOL_STR(skb_partial_csum_set) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x5713f1c3, __VMLINUX_SYMBOL_STR(netif_napi_del) },
	{ 0xc9ec4e21, __VMLINUX_SYMBOL_STR(free_percpu) },
	{ 0xa05629f, __VMLINUX_SYMBOL_STR(vca_virtqueue_disable_cb) },
	{ 0x278fe526, __VMLINUX_SYMBOL_STR(skb_trim) },
	{ 0x1aa53af, __VMLINUX_SYMBOL_STR(__netdev_alloc_skb) },
	{ 0xf4b8f44d, __VMLINUX_SYMBOL_STR(vca_virtqueue_detach_unused_buf) },
	{ 0xfe7c4287, __VMLINUX_SYMBOL_STR(nr_cpu_ids) },
	{ 0x95e2cb96, __VMLINUX_SYMBOL_STR(netif_tx_wake_queue) },
	{ 0x665cd765, __VMLINUX_SYMBOL_STR(netif_tx_stop_all_queues) },
	{ 0x4d22d4b4, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x4ae8352d, __VMLINUX_SYMBOL_STR(vca_virtqueue_get_vring_size) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x844d1c25, __VMLINUX_SYMBOL_STR(ethtool_op_get_link) },
	{ 0x3c3fce39, __VMLINUX_SYMBOL_STR(__local_bh_enable_ip) },
	{ 0xe1ae7ce5, __VMLINUX_SYMBOL_STR(vca_unregister_virtio_driver) },
	{ 0xf8a591c8, __VMLINUX_SYMBOL_STR(vca_register_virtio_driver) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x9cd66d59, __VMLINUX_SYMBOL_STR(free_netdev) },
	{ 0x3d679621, __VMLINUX_SYMBOL_STR(register_netdev) },
	{ 0xee255fa6, __VMLINUX_SYMBOL_STR(vca_virtqueue_add_outbuf) },
	{ 0xaa121f07, __VMLINUX_SYMBOL_STR(netif_receive_skb) },
	{ 0x5792f848, __VMLINUX_SYMBOL_STR(strlcpy) },
	{ 0x9ef353f2, __VMLINUX_SYMBOL_STR(vca_virtqueue_enable_cb_prepare) },
	{ 0x170c2505, __VMLINUX_SYMBOL_STR(netif_set_real_num_rx_queues) },
	{ 0xdfe025a8, __VMLINUX_SYMBOL_STR(netif_set_real_num_tx_queues) },
	{ 0x4e729a48, __VMLINUX_SYMBOL_STR(netif_napi_add) },
	{ 0xa59b4c3, __VMLINUX_SYMBOL_STR(vca_virtqueue_add_sgs) },
	{ 0x1fe9f800, __VMLINUX_SYMBOL_STR(unregister_cpu_notifier) },
	{ 0xd52218b3, __VMLINUX_SYMBOL_STR(netif_device_attach) },
	{ 0x17309c92, __VMLINUX_SYMBOL_STR(__free_pages) },
	{ 0xaf8eb12f, __VMLINUX_SYMBOL_STR(vca_virtio_check_driver_offered_feature) },
	{ 0x76c022c7, __VMLINUX_SYMBOL_STR(netif_device_detach) },
	{ 0xe7d8c13d, __VMLINUX_SYMBOL_STR(vca_virtqueue_kick) },
	{ 0x8b43159b, __VMLINUX_SYMBOL_STR(register_cpu_notifier) },
	{ 0x938dd3de, __VMLINUX_SYMBOL_STR(eth_prepare_mac_addr_change) },
	{ 0x13d90a53, __VMLINUX_SYMBOL_STR(__napi_schedule) },
	{ 0x70cd1f, __VMLINUX_SYMBOL_STR(queue_delayed_work_on) },
	{ 0x73032782, __VMLINUX_SYMBOL_STR(cpu_possible_mask) },
	{ 0xd7d79132, __VMLINUX_SYMBOL_STR(put_online_cpus) },
	{ 0x25db8951, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0x40e708d2, __VMLINUX_SYMBOL_STR(napi_complete_done) },
	{ 0x3e3e19b5, __VMLINUX_SYMBOL_STR(eth_type_trans) },
	{ 0xc435348c, __VMLINUX_SYMBOL_STR(netdev_notify_peers) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x247a12c5, __VMLINUX_SYMBOL_STR(eth_commit_mac_addr_change) },
	{ 0x3efb35c9, __VMLINUX_SYMBOL_STR(get_online_cpus) },
	{ 0xb19a5453, __VMLINUX_SYMBOL_STR(__per_cpu_offset) },
	{ 0x2a18c74, __VMLINUX_SYMBOL_STR(nf_conntrack_destroy) },
	{ 0xf6ebc03b, __VMLINUX_SYMBOL_STR(net_ratelimit) },
	{ 0x1172517b, __VMLINUX_SYMBOL_STR(eth_validate_addr) },
	{ 0xb6244511, __VMLINUX_SYMBOL_STR(sg_init_one) },
	{ 0xf522268a, __VMLINUX_SYMBOL_STR(vca_virtqueue_is_broken) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x53569707, __VMLINUX_SYMBOL_STR(this_cpu_off) },
	{ 0x63c4d61f, __VMLINUX_SYMBOL_STR(__bitmap_weight) },
	{ 0xab9f76ba, __VMLINUX_SYMBOL_STR(dev_warn) },
	{ 0x86872d57, __VMLINUX_SYMBOL_STR(unregister_netdev) },
	{ 0x7e9f6608, __VMLINUX_SYMBOL_STR(netif_wake_subqueue) },
	{ 0x2e0d2f7f, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x34d7675, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0x8ceeab74, __VMLINUX_SYMBOL_STR(vca_virtqueue_enable_cb_delayed) },
	{ 0x97565515, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0x6e720ff2, __VMLINUX_SYMBOL_STR(rtnl_unlock) },
	{ 0xf6893f5, __VMLINUX_SYMBOL_STR(alloc_etherdev_mqs) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=vca_virtio_ring,vca_virtio";

MODULE_ALIAS("virtio:d00000001v*");

MODULE_INFO(srcversion, "2399D168E49D712B74C0884");
