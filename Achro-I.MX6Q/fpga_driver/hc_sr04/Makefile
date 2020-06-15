DEV := hc-sr04
DD := fpga_$(DEV)_driver
CC = arm-none-linux-gnueabi-gcc
KDIR := /root/work/achroimx6q/kernel
PWD := $(shell pwd)
obj-m := $(DD).o

all:	$(DD)

$(DD):	$(DD).c
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm
clean:
	rm -rf *.ko *.mod.* *.order *.symvers *.o
new:
	$(MAKE) clean
	$(MAKE)
