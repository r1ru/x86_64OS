AR = ar
CC = clang
CFLAGS = -Wall -g -target x86_64-elf -ffreestanding -fno-pie -mno-red-zone -fno-stack-protector \
		-I include -I../kernel/include -I../Uefi/Include
OBJS = 	pci/pci.o pci/asmfunc.o \
		port/manager.o \
		slot/manager.o \
		xhci/driver.o xhci/registers.o xhci/txRing.o xhci/rxRing.o xhci/interruptHandler.o xhci/context.o \
		device/device.o device/manager.o device/descriptor.o \
		memory/allocator.o \
		usbError.o 

usbDriver.a: $(OBJS)
	$(AR) rcs $@ $^

asmfunc.o : asmfunc.s 
	$(CC) -c -o $@ $<

%.o : %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(shell find ./ -name "*.o" -o -name "*.a")

.PHONY : clean 
	