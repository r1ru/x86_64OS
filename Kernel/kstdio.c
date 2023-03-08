#include <kstdio.h>

#define BUF_SIZE 100

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))

#define ALT_FORM   (1U<<'#'-' ')
#define ZERO_PAD   (1U<<'0'-' ')
#define LEFT_ADJ   (1U<<'-'-' ')
#define PAD_POS    (1U<<' '-' ')
#define MARK_POS   (1U<<'+'-' ')

#define FLAGMASK (ALT_FORM|ZERO_PAD|LEFT_ADJ|PAD_POS|MARK_POS)

enum {
    BARE, PTR, INT, UINT, CHAR, UIPTR 
};

#define S(x) [(x) - 'A']

static const unsigned char states[]['z' - 'A' + 1] = {
    { // 0: bare types
        S('d') = INT, 
        S('u') = UINT, S('x') = UINT, S('X') = UINT,
        S('c') = CHAR, 
        S('s') = PTR, S('p') = UIPTR 
    }
};

#define OOB(x) ((unsigned)(x)-'A' > 'z'-'A')

union arg {
	uintmax_t i;
	void *p;
};

static void pop_arg(union arg *arg, int type, kva_list ap) {
    switch (type) {
	       case PTR:	arg->p = (void*)__kbuiltin_va_arg(ap);
	break; case INT:	arg->i = __kbuiltin_va_arg(ap);
	break; case UINT:	arg->i = __kbuiltin_va_arg(ap);
	break; case CHAR:	arg->i = (signed char)__kbuiltin_va_arg(ap);
	break; case UIPTR:	arg->i = (uintptr_t)__kbuiltin_va_arg(ap);
	}
}

static void out(struct cookie *c, const char *s, unsigned int l) {
    kmemcpy(c->s, s, l);
    c->s += l;
    c->n -= l;
    *c->s = 0; // null terminate
}   

static void pad(struct cookie *cookie, char c, int w, int l, int fl) {
    char pad[BUF_SIZE];
    if(fl & (LEFT_ADJ | ZERO_PAD) || l >= w) return;
    l = w - l;
    kmemset(pad, c, l); 
    out(cookie, pad, l);
}

static const char xdigits[16] = "0123456789ABCDEF";

static char *fmt_x(uintmax_t x, char *s, int lower) {
    for(; x; x >>= 4) *--s = xdigits[(x&15)] | lower;
    return s;
}

static char *fmt_u(uintmax_t x, char *s) {
	for (; x; x/=10) *--s = '0' + x%10;
	return s;
}

static int isdigit (int c) {return '0' <= c && c <= '9';}

static int getint(char **s) {
    int i;
    for(i=0; isdigit(**s); (*s)++) {
        if(i > BUF_SIZE / 10U || **s - '0' > BUF_SIZE - 10 * i) i = -1;
        else i = 10 * i + (**s - '0');
    }
    return i;
}

static int printf_core(struct cookie *c, const char* fmt, kva_list ap){
    char *a, *z, *s = (char*)fmt;
    unsigned int fl;
    int w, p, xp;
    unsigned int st;
    int cnt = 0, l = 0;
    union arg arg;
    char buf[BUF_SIZE]; // TOOD: ここ直す
    const char *prefix;
    int t, pl;

    for(;;) {
        cnt += l;
        if(!*s) break;
        
        // '%'までの文字列を出力する
        for(a = s; *s && *s != '%'; s++);
        for (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        if(z - a > BUF_SIZE - cnt) goto overflow;
        l = z - a;
        if(c) out(c, a, l);
        if(l) continue;

        // '%'を読み飛ばす
        s++;

        // flag文字('#','0',' ', '+','-')を読む
        for(fl = 0; (unsigned int)*s - ' ' < 32 && (FLAGMASK&(1U << *s - ' ')); s++)
            fl |= 1U << *s - ' ';

        // 最小フィールド幅を読む。int型でない場合はエラー
        if((w = getint(&s)) < 0) goto overflow;

        // 精度を読む
        if(*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } else {
            p = -1;
            xp = 0;
        }

        // 変換指定子に対応する引数の方を特定
        st = BARE;
        if(OOB(*s)) goto inval;
        st = states[st]S(*s++);
        if(!st) goto inval;

        // 対応する引数を取ってくる
        if(c) pop_arg(&arg, st, ap);
        
        if(!c) continue;

        z = buf + sizeof(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        // -と0が同時に指定された場合は0を無視
        if(fl & LEFT_ADJ) fl &= ~ZERO_PAD;

        switch(t) {
            case 'p':
                p = MAX(p, 2 * sizeof(void *)); // 1byteは16進数2桁
                t = 'x';
                fl |= ALT_FORM; // '0x'を付加する
            case 'x': case 'X':
                a = fmt_x(arg.i, z, t & 32);
                if(arg.i && (fl & ALT_FORM)) prefix += (t>>4), pl = 2;
                if(0){
            case 'd':
                pl = 1;
                if(arg.i > INTMAX_MAX) // この条件はarg.iが負の時に真になる。
                    arg.i = - arg.i;
                else if(fl & MARK_POS) 
                    prefix++;
                else if(fl & PAD_POS)
                    prefix += 2;
                else 
                    pl = 0;
            case 'u':
                a = fmt_u(arg.i, z);
                }
                // 整数変換(d, u, X)共通の処理
                if(xp && p < 0) goto overflow;
                if(xp) fl &= ~ZERO_PAD; // 精度指定がある場合は0フラグを無視する
                if(!arg.i && !p) { // 0を表示する際に明示的に精度が0に指定されていた場合は空白文字を出力
                    a = z;
                    break;
                }
                p = MAX(p, z - a + !arg.i); // 精度は表示する"最小の"桁数。arg.iが0の時は1足される。fmt_xが0を書き込まないから。
                break;
            case 'c':
                *(a = z -(p = 1)) = arg.i;
                fl &= ~ZERO_PAD;
                break;
            case 's':
                a = arg.p ? arg.p : "(null)"; // 引数がNULLの場合にSegmentationFaultを出さないため。
                z = a + kstrnlen(a, p < 0 ? BUF_SIZE - 1 : p);
                if(p < 0 && *z) goto overflow; // 精度指定がなく、長さがBUF_ZIZEを超える場合
                p = z - a;
                fl &= ~ZERO_PAD;
                break;
        }

        if(p < z - a) p = z - a;
        if(p > BUF_SIZE - pl) goto overflow;
        if(w < pl + p) w = pl + p;
        if(w > BUF_SIZE - cnt) goto overflow;

        pad(c, ' ', w, pl + p, fl);
        out(c, prefix, pl);
        pad(c, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(c, '0', p, z - a, 0);
        out(c, a, z - a);
        pad(c, ' ', w, pl + p, fl ^ ZERO_PAD);

        l = w;
    }

    return cnt;

    inval:  
        return -1;
    overflow:   
        return -1;

}

// TODO: va_copyを実装する
int kvsnprintf(char *restrict s, unsigned int n, const char *restrict fmt, kva_list ap) {
    struct cookie c = {.s = s, .n = n - 1}; // TODO: ここ直す
    
    if(n > BUF_SIZE)
        return -1;
    
    if(printf_core(NULL, fmt, ap) < 0)
        return -1;

    return printf_core(&c, fmt, ap);
}

// TODO: consoleを実装する。WriteAsciiで小文字の場合は大文字で表示するように変える。
int kprintf(const char *fmt, ...) {
    char buf[BUF_SIZE];
    kva_list ap;
    int n;
    static int y = 66;
    
    __builtin_va_start(ap, 1);
    n = kvsnprintf(buf, BUF_SIZE, fmt, ap);

    WriteString(0, y, buf, &white);
    y += FONT_HEIGHT;

    return n;
}