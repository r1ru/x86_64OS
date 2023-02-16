AR = ar
CC = clang
CFLAGS = -Wall -target x86_64-pc-win32-coff -mno-red-zone -fno-stack-protector \
		 -fshort-wchar -I../Uefi/Include
OBJS = Guid.o UefiLib.o 

UefiLib.a : $(OBJS)
	$(AR) rcs $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -IInclude -c -o $@ $<

clean:
	rm -f *.o *.a

.PHONY: clean
