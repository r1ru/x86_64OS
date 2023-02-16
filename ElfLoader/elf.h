#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>

#define EI_NIDENT   (16)

typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Xword;

typedef struct {
    uint8_t e_ident[EI_NIDENT];
    Elf64_Half      e_type;
    Elf64_Half      e_machine;
    Elf64_Word      e_version;
    Elf64_Addr      e_entry;
    Elf64_Off       e_phoff;
    Elf64_Off       e_shoff;
    Elf64_Word      e_flags;
    Elf64_Half      e_ehsize;
    Elf64_Half      e_phentsize;
    Elf64_Half      e_phnum;
    Elf64_Half      e_shentsize;
    Elf64_Half      e_shnum;
    Elf64_Half      e_shstrndx;
} Elf64_Ehdr;

// Fields in the e_ident array
#define EI_MAG0     0
#define ELFMAG0     0x7f

#define EI_MAG1     1
#define ELFMAG1     'E'

#define EI_MAG2     2
#define ELFMAG2     'L'

#define EI_MAG3     3
#define ELFMAG3     'F'

#define EI_CLASS        4
#define ELFCLASSNONE    0   // Invalid class
#define ELFCLASS32      1   // 32-bit objects
#define ELFCLASS64      2   // 64-bit objects

#define EI_DATA         5
#define ELFDATANONE     0   // Invalid data encoding
#define ELFDATA2LSB     1   // little endian
#define ELFDATA2MSB     2   // big endian

// Legal values for e_type
#define ET_EXEC         2

typedef struct {
    Elf64_Word  p_type;
    Elf64_Word  p_flags;
    Elf64_Off   p_offset;
    Elf64_Addr  p_vaddr;
    Elf64_Addr  p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

#define PT_LOAD 1

#endif 