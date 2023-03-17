CC = clang
CFLAGS = -Wall -g -target x86_64-elf -ffreestanding -fno-pie -mno-red-zone -fno-stack-protector \
		-I include -I../Kernel/Arch/x64/Include -I../Kernel/Include -I../Uefi/Include

all : pci.o asmfunc.o
.PHONY : all

pci.o : pci.c 
	$(CC) $(CFLAGS) -c -o $@ $<

asmfunc.o : asmfunc.s 
	$(CC) -c -o $@ $<

clean:
	rm -f *.o 

.PHONY : clean 
	