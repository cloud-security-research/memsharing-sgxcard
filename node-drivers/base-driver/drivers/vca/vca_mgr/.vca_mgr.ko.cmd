cmd_/vca-software-stack/node-drivers/base-driver/drivers/vca/vca_mgr/vca_mgr.ko := ld -r -m elf_x86_64 -T ./scripts/module-common.lds --build-id -z noexecstack -z relro -z now -o /vca-software-stack/node-drivers/base-driver/drivers/vca/vca_mgr/vca_mgr.ko /vca-software-stack/node-drivers/base-driver/drivers/vca/vca_mgr/vca_mgr.o /vca-software-stack/node-drivers/base-driver/drivers/vca/vca_mgr/vca_mgr.mod.o