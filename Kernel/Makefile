CC = clang
CFLAGS = -O2 -Wall -g -target x86_64-elf -ffreestanding -fno-pie -mno-red-zone -fno-stack-protector \
		-I../Uefi/Include -I./Arch/x64/Include
LDFLAGS = --entry _start -z norelro --image-base 0x100000 --static -no-pie		
OBJS = kernel.o graphics.o font.o ./Arch/x64/kbuiltin.o kstring.o kstdio.o console.o

kernel.elf : $(OBJS) 
	ld.lld $(LDFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -IInclude -c -o $@ $<

./Arch/x64/kbuiltin.o : ./Arch/x64/kbuiltin.c
	make -C ./Arch/x64/

clean:
	rm -f *.o *.elf
	make -C ./Arch/x64 clean

.PHONY: clean