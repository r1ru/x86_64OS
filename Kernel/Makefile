CC = clang
CFLAGS = -O2 -Wall -g -target x86_64-elf -ffreestanding -fno-pie -mno-red-zone -fno-stack-protector
INCLUDES = -IInclude -I../Uefi/Include -I./Arch/x64/Include -I../usb/include
LDFLAGS = --entry _start -z norelro --image-base 0x100000 --static -no-pie		
OBJS = 	kernel.o graphics.o font.o ./Arch/x64/kbuiltin.o kstring.o kstdio.o console.o segment.o \
		interrupt.o interruptMessageQueue.o asmfunc.o
DEPENDS = ../usb/usbDriver.a

kernel.elf : $(OBJS) $(DEPENDS)
	ld.lld $(LDFLAGS) -o $@ $^

$(DEPENDS) : 
	make -C ../usb

asmfunc.o : asmfunc.s 
	$(CC) -c -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

./Arch/x64/kbuiltin.o : ./Arch/x64/kbuiltin.c
	make -C ./Arch/x64/

clean:
	rm -f *.o *.elf
	make -C ./Arch/x64 clean
	make -C ../usb clean 

.PHONY: clean