#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

struct param {
	char uc:1;
	char sign;			/* '-' 				 */
	unsigned int base;	/* number base, e.g 8, 10, 16*/
	char *bf;			/* buffer for output */
};
typedef void (*putcf) (void *, char);

struct _vsprintf_putcf_data {
	char *dest;
	size_t num_chars;
};

static void _vsprintf_putcf(void *p, char c) {
	struct _vsprintf_putcf_data *data = (struct _vsprintf_putcf_data *)p;
	data->dest[data->num_chars ++] = c;
}

static void _printf_putc(void *p, char c) {  
	_putc(c);
}

static void putchw(void *putp, putcf putf, struct param *p) {
	char ch;
	char *bf = p->bf;
	while((ch = *bf++)) 
		putf(putp, ch);
}

static void ui2a(unsigned int num, struct param *p) {
	int n = 0;
	unsigned int d = 1;
	char *bf = p->bf;
	while (num / d >= p->base)
		d = d * p->base;

	while (d != 0) {
		int dgt = num /d;
		num = num % d;
		d = d / p->base;
		if (n || dgt > 0 || d == 0) {
			*bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
			++n;
		}
	}
	*bf = 0;
}
static void i2a (int num, struct param *p) {
	//printf("num = %d\n", num);
	if (num < 0) {
		num = -num;
		p->sign = '-';
	}
	ui2a(num, p);
}


void my_format(void *putp, putcf putf, const char *fmt, va_list va) {
	struct param p;
	char bf[12];	/* int = 32b */
	char ch;
	p.bf = bf;

	while ((ch = *(fmt++))) {
		if (ch != '%') {
			putf(putp, ch);
		} else {
			ch = *(fmt++);
			switch (ch) {
				case 'd':
					p.base = 10;
					p.uc = 0;
					i2a(va_arg(va, int), &p);
					putchw(putp, putf, &p);
					break;
				case 'x' :
					p.base = 16;
					p.uc = 0;
					i2a(va_arg(va, int), &p);
					putchw(putp, putf, &p);
					break;
				case 's':
					p.bf = va_arg(va, char *);
					putchw(putp, putf, &p);
				    p.bf = bf; //???
					break;
				default : break;
			}
		}

	}
}


int vsprintf(char *out, const char *fmt, va_list ap) {
	struct _vsprintf_putcf_data data;
	data.dest = out;
	data.num_chars = 0;
	my_format(&data, _vsprintf_putcf, fmt, ap);
	data.dest[data.num_chars] = '\0';

	return data.num_chars;
}
static inline void _putstr(char *s) {
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	int ret;
	
	va_start(ap, fmt);
	ret = vsprintf(out, fmt, ap);
	va_end(ap);

	return ret;
}

int printf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	my_format(NULL, _printf_putc, fmt, ap);
	va_end(ap);

	/*
		0x3F8 
		printf -> _putc(ch) -> putchar(0x3F8, ch) -> outb(addr, ch) -> *addr = ch
	 */
	return 0;
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