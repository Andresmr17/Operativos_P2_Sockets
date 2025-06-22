# Kernel module part
obj-m += usb_driver_arduino.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

# Library part
LIB_NAME = libuart.a
LIB_SRC  = uart_interface.c ../Servidor/uart_control.c
LIB_OBJ  = $(LIB_SRC:.c=.o)

# Targets
all: driver lib

driver:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

lib: $(LIB_OBJ)
	ar rcs $(LIB_NAME) $(LIB_OBJ)

%.o: %.c
	gcc -Wall -O2 -c $< -o $@

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.o *.ko *.mod.* modules.order Module.symvers $(LIB_NAME)
