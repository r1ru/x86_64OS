CC = clang
CFLAGS = -O2 -Wall -g -target x86_64-elf -ffreestanding -mno-red-zone -I../Uefi/Include
LDFLAGS = --entry _start -z norelro --image-base 0x100000 --static 

kernel.elf : kernel.o
	ld.lld $(LDFLAGS) -o $@ $<

kernel.o : kernel.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.elf

.PHONY: clean