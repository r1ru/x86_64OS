#include "ElfLoader.h"

// ELF64 excutableかチェックする
#define IS_EIF64EX(ehdr) \
    ((ehdr)->e_ident[EI_MAG0] == ELFMAG0 && \
    (ehdr)->e_ident[EI_MAG1] == ELFMAG1 && \
    (ehdr)->e_ident[EI_MAG2] == ELFMAG2 && \
    (ehdr)->e_ident[EI_MAG3] == ELFMAG3 && \
    (ehdr)->e_ident[EI_CLASS] == ELFCLASS64 && \
    (ehdr)->e_ident[EI_DATA] == ELFDATA2LSB && \
    (ehdr)->e_type == ET_EXEC)  

void UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status;
    EFI_FILE_PROTOCOL *root, *file;

    // Init UefiLib
    UefiInit(ImageHandle, SystemTable);

    // Locate "kernel.elf"
    status = OpenRootDir(ImageHandle, &root);
    assert(status, L"OpenRootDir");

    status = root->Open(root, &file, L"kernel.elf", EFI_FILE_MODE_READ, 0);
    assert(status, L"root->Open");

    // Get file info
    UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 file_info_buffer[file_info_size];

    status = file->GetInfo(file, &gEfiFileInfoGuid, &file_info_size, file_info_buffer);
    assert(status, L"file->GetInfo");

    EFI_FILE_INFO *file_info = (EFI_FILE_INFO*)file_info_buffer;
    UINTN file_size = file_info->FileSize;

    // Allocate memory pool and copy file contents
    UINT8 *file_buffer;
    status = gBS->AllocatePool(
        EfiLoaderData,
        file_size,
        (VOID **)&file_buffer
    );
    assert(status, L"AllocatePool");

    status = file->Read(file, &file_size, (VOID *)file_buffer);
    assert(status, L"file->Read");

    // Read program headers
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_buffer;

    if(!IS_EIF64EX(ehdr)){
        puts(L"This File is not ELF64 executable\r\n");
        while(1) asm volatile("hlt");
    }

    // Calucurate address range
    UINT64 start = MAX_UINT64, end = 0;
    for(Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr *phdr = (Elf64_Phdr*)(file_buffer + ehdr->e_phoff + ehdr->e_phentsize * i);
        if(phdr->p_type != PT_LOAD) continue;

        start = phdr->p_vaddr < start ? phdr->p_vaddr : start;
        end = end < (phdr->p_vaddr + phdr->p_memsz) ? (phdr->p_vaddr + phdr->p_memsz) : end;
    }

    // Allocate pages
    UINTN num_pages = (end - start + 0xff) / 0x1000;

    status = gBS->AllocatePages(
        AllocateAddress,
        EfiLoaderData,
        num_pages,
        &start
    );
    assert(status, L"gBS->AllocatePages");

    // Copy LOAD Segments
    for(Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr *phdr = (Elf64_Phdr*)(file_buffer + ehdr->e_phoff + ehdr->e_phentsize * i);
        if(phdr->p_type != PT_LOAD) continue;
        Memcpy((UINT8 *)phdr->p_vaddr, file_buffer + phdr->p_offset, phdr->p_filesz);

        // Init .bss section
        UINTN remain_bytes = phdr->p_memsz - phdr->p_filesz;
        Bzero((UINT8 *)phdr->p_vaddr + phdr->p_filesz, remain_bytes);
    }

    // Locate entry point
    EntryPointType *entry_point = (EntryPointType*)ehdr->e_entry;

    // Free allocated memory pool
    status = gBS->FreePool((VOID *)file_buffer);
    assert(status, L"gBS->FreePool");

    // Exit BootServices
    status = ExitBootServices(ImageHandle);
    assert(status, L"ExitBootServices");
    
    // Prepare argments 
    FrameBufferInfo info = {
        GOP->Mode->FrameBufferBase,
        GOP->Mode->Info->HorizontalResolution,
        GOP->Mode->Info->VerticalResolution,
        GOP->Mode->Info->PixelsPerScanLine
    };

    switch(GOP->Mode->Info->PixelFormat) {
        case PixelRedGreenBlueReserved8BitPerColor:
            info.pixel_format = kPixelRedGreenBlueReserved8BitPerColor;
            break;

        case PixelBlueGreenRedReserved8BitPerColor:
            info.pixel_format = kPixelBlueGreenRedReserved8BitPerColor;
            break;
        
        default:
            puts(L"Unsupported PixelFormat\r\n");
            while(1) asm volatile("hlt");

    }

    // Start Kernel
    entry_point(&info);
    
    while(1) asm volatile("hlt");
}