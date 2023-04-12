CC = clang
CFLAGS = -O2 -Wall -g -target x86_64-elf -ffreestanding -fno-pie -mno-red-zone -fno-stack-protector
INCLUDES = -IInclude -I../Uefi/Include -I../usb/include
LDFLAGS = --entry _start -z norelro --image-base 0x100000 --static -no-pie		
OBJS = 	kernel.o graphics.o font.o kstring.o kstdio.o console.o segment.o \
		interrupt.o interruptMessageQueue.o asmfunc.o logger.o 
DEPENDS = ../usb/usbDriver.a

kernel.elf : $(OBJS) $(DEPENDS)
	ld.lld $(LDFLAGS) -o $@ $^

$(DEPENDS) : 
	make -C ../usb

asmfunc.o : asmfunc.s 
	$(CC) -c -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f *.o *.elf
	make -C ../usb clean 

.PHONY: clean