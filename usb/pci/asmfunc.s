.intel_syntax noprefix
.section .text

# void ioOut32(uint16_t addr, uint32_t data); 
.global ioOut32
ioOut32:
    xor edx, edx
    mov dx, di 
    mov eax, esi
    out dx, eax
    ret 

# uint32_t ioIn32(uint16_t addr);
.global ioIn32
ioIn32:
    xor edx, edx
    mov dx, di
    in eax, dx
    ret
