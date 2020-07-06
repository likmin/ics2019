#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
struct param {
	char lz:1;
	char zlt:1;
	char uc:1;
	char align_left:1;
	unsigned int width;
	char sign;
	unsigned int base;	/* number base, e.g 8, 10, 16*/
	char *bf;			/* buffer for output */
};
static inline void chartobuf(char *p, char c) {
	*p = c;
	p++;
}
static inline void strtobuf(char *p, char *str) {
	while(str){
		*p = *str;
		p++; str++;
	}
}

static inline void putstr(const char *s) {
  for (; *s; s ++) _putc(*s);
}

static void ui2a(unsigned int num, struct param *p)
{
    int n = 0;
    unsigned int d = 1;
    char *bf = p->bf;
    while (num / d >= p->base)
        d *= p->base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= p->base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}

static void i2a(int num, struct param *p)
{
    if (num < 0) {
        num = -num;
        p->sign = '-';
    }
    ui2a(num, p);
}



int printf(const char *fmt, ...) {
  //for (; *fmt; fmt ++) _putc(*fmt);
  char p[128]; 
  
  va_list va;
  va_start(va, fmt);
  vsprintf(p, fmt, va);
  va_end(va);

  putstr(fmt);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	char ch;
  	struct param p;
	char bf[12]; /*for 32bit number is enough*/
	p.bf = bf;

	while((ch = *(fmt++))) {
		if (ch != '%') {
			chartobuf(out, ch);
		} else {
			switch(ch) {
				case 's':
					strtobuf(out, va_arg(ap, char *));						
					break;
				case 'd':
					i2a(va_arg(ap, int), &p);				    
					strtobuf(out, p.bf);
			 		break;	
				default: assert(0);
			}
		}
	}
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  
  va_list aptr;
	int ret;
	
	va_start(aptr, fmt);
	ret = vsprintf(out, fmt, aptr);
	va_end(aptr);
	return ret;
}

/* snprintf()用于将格式化的数据写入字符串 
 * out: 为要写入的字符串
 * n  : 要写入的字符的最大数据，超过n会被截断
 * fmt: 格式化字符串
 */
int snprintf(char *out, size_t n, const char *fmt, ...) {

  return 0;
}

#endif
