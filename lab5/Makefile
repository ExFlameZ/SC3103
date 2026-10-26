obj-m += hello_lkm.o gpio_lkm.o
KDIR ?= /lib/modules/$(shell uname -r)/build

all: gpio
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

gpio: gpio.c
	gcc -Wall -o gpio gpio.c

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
	rm -f gpio
