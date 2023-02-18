CC = clang
CFLAGS = -O2 -Wall -g -target x86_64-elf -ffreestanding -mno-red-zone -I../Uefi/Include
LDFLAGS = --entry _start -z norelro --image-base 0x100000 --static 
OBJS = kernel.o graphics.o font.o

kernel.elf : $(OBJS)
	ld.lld $(LDFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -IInclude -c -o $@ $<

clean:
	rm -f *.o *.elf

.PHONY: clean