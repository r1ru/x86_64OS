CC = clang
CFLAGS = -Wall -target x86_64-pc-win32-coff -mno-red-zone -fno-stack-protector \
		 -fshort-wchar -I../Uefi/Include
LD = lld-link
LDFLAGS = /subsystem:efi_application
LIBPATH = ../UefiLib
INLCLUDEPATH = $(LIBPATH)/Include 

BOOTX64.EFI: ElfLoader.o $(LIBPATH)/UefiLib.a
	$(LD) $(LDFLAGS) /entry:UefiMain /out:$@ $^

ElfLoader.o : ElfLoader.c
	$(CC) $(CFLAGS) -I$(INLCLUDEPATH) -c -o $@ $<

$(LIBPATH)/UefiLib.a :
	make -C $(LIBPATH)

clean:
	rm -f *.o *.EFI
	make -C $(LIBPATH) clean

.PHONY: clean

