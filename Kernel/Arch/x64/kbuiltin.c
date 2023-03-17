#include <kbuiltin.h>

/* TODO:   
* ・originalのva_startのように最後の引数を受け取り、可変長引数の前に複数の引数を取れるようにする
* ・floating point registerを使えるようにする? 
*/

/*
* @param n format文字列の前に渡された引数の数
*/
void __kbuiltin_va_start(__kbuiltin_va_list ap, uint64_t n) {
    ap->gp_offset = n * 8;
    ap->fp_offset = 0;
    ap->overflow_arg_area = 0;

    __asm__ volatile(
        "mov rax, qword ptr [rbp];" // saved rbp
        "lea rax, [rax - 0xb0];"
        "mov %0, rax"
        : "=m" (ap->reg_save_area)
        : 
        : "rax"
    );
}

// floating point registerの使用は想定されていない
uint64_t __kbuiltin_va_arg(__kbuiltin_va_list ap) {
    if(ap->gp_offset < 48){
        ap->gp_offset += 8;
        return *(uint64_t *)(ap->reg_save_area + (ap->gp_offset - 8));
    }
    else {
        // これはx64だから動くコード。x86だと型名が必要になる。
        return *(uint64_t *)(ap->overflow_arg_area)++;
    }
}


