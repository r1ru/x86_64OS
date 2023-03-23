CC = clang
CFLAGS = -Wall -g -target x86_64-elf -ffreestanding -fno-pie -mno-red-zone -fno-stack-protector \
		-I include -I../Kernel/Arch/x64/Include -I../Kernel/Include -I../Uefi/Include
OBJS = pci.o asmfunc.o xhcDriver.o

all : $(OBJS)
.PHONY : all

asmfunc.o : asmfunc.s 
	$(CC) -c -o $@ $<

%.o : %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o 

.PHONY : clean 
	