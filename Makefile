#
# Makefile for the LTTng clock plugin example module.
#

ifneq ($(KERNELRELEASE),)

ifeq ($(LTTNG_MODULES_DIR),)
$(error Please specify LTTNG_MODULES_DIR=/path/to/lttng-modules)
endif

ccflags-y=-I$(LTTNG_MODULES_DIR)
KBUILD_EXTRA_SYMBOLS=$(LTTNG_MODULES_DIR)/Module.symvers

obj-m += lttng-clock-plugin-example.o

else # KERNELRELEASE
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	CFLAGS = $(EXTCFLAGS)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

%.i: %.c
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@
endif # KERNELRELEASE
