obj-m += somnath_test.o
KBUILD_EXTRA_SYMBOLS := /vca_exp/exp/unmodified_modules/Module.symvers 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
