cmd_/vca-software-stack/node-drivers/base-driver/plx87xx.o := ld -m elf_x86_64  -z noexecstack -z relro -z now  -r -o /vca-software-stack/node-drivers/base-driver/plx87xx.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_main.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_boot.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_hw.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_debugfs.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_procfs.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_intr.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_alm.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_lbp.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_hw_ops_blockio.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_hw_ops_vca_mgr.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_hw_ops_vca_mgr_extd.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_hw_ops_vca_csa.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_hw_ops_vca_csm.o /vca-software-stack/node-drivers/base-driver/vca/plx87xx/plx_hw_ops_vop.o ; scripts/mod/modpost /vca-software-stack/node-drivers/base-driver/plx87xx.o