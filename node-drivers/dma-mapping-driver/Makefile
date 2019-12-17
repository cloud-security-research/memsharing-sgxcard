obj-m += sgx5.o

KBUILD_EXTRA_SYMBOLS := $(PWD)/../base-driver/drivers/Module.symvers

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
