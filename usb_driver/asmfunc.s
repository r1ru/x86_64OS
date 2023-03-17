.intel_syntax noprefix
.section .text
.global ioOut32

# void ioOut32(uint16_t addr, uint32_t data); 
ioOut32:
    xor edx, edx
    mov dx, di 
    mov eax, esi
    out dx, eax
    ret 

.global ioIn32

# uint32_t ioIn32(uint16_t addr);
ioIn32:
    xor edx, edx
    mov dx, di
    in eax, dx
    ret