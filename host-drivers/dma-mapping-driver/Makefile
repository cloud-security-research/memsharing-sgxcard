obj-m += sgx5_0.o
obj-m += sgx5_1.o
obj-m += sgx5_2.o

obj-m += sgx5_3.o
obj-m += sgx5_4.o
obj-m += sgx5_5.o

KBUILD_EXTRA_SYMBOLS := $(PWD)/../base-driver/Module.symvers
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
