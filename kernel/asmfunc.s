.intel_syntax noprefix 
.section .text

# void loadGDT(uint16_t limit, uint64_t offset)
.global loadGDT
loadGDT:
    push rbp
    mov rbp, rsp
    push rsi
    push di 
    lgdt [rsp]
    leave
    ret 

# void setCSSS(uint16_t cs, uint16_t ss)
.global setCSSS
setCSSS:
    push rbp
    mov rbp, rsp
    mov ss, si
    lea rax, .L0[rip]
    push di
    push rax
    retfq
.L0:
    leave 
    ret

# uint16_t getCS(void)
.global getCS
getCS:
    xor eax, eax
    mov ax, cs
    ret

# void _loadIDT(uint16_t limit, uint64_t offset)
.global _loadIDT
_loadIDT:
    push rbp
    mov rbp, rsp
    push rsi 
    push di 
    lidt [rsp]
    leave 
    ret 