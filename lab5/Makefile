obj-m += hello_lkm.o gpio_lkm.o
KDIR ?= /lib/modules/$(shell uname -r)/build

all: gpio_polling gpio_event
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

gpio_polling gpio_event: %: %.c
	gcc -Wall -o $@ $<

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
	rm -f gpio_polling gpio_event
