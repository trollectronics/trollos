#include <stdarg.h>
#include "terminal.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define PRINT_TYPE(type, argtype, sig) do {\
	s = int_to_string((type) va_arg(va, argtype), buf + 24, base, (sig)); \
	j = 24 + buf - s; \
	if(!width) \
		width = j; \
	else \
		while(width > j)  { \
			terminal_putc_simple(pad); \
			width--; \
		} \
	terminal_put_counted(s, width); \
} while(0)



static char *int_to_string(unsigned long long int n, char *s, int base, int sig) {
	char i;
	if(sig && (((long long int) n ) < 0)) {
		n = -n;
	} else
		sig = 0;
	
	do {
		s--;
		i = n  % base;
		i += (i > 9 ? 'A' - 10 : '0');
		*s = i;
		n /= base;
	} while(n);
	if(sig)
	*--s = '-';
	return s;
}

int vprintf(char *format, va_list va) {
	unsigned char pad, c;
	int i;
	unsigned int j;
	
	enum {
		LENGTH_CHAR,
		LENGTH_SHORT,
		LENGTH_INT,
		LENGTH_LONG,
		LENGTH_LONG_LONG,
		LENGTH_INTMAX_T,
		LENGTH_SIZE_T,
		LENGTH_PTRDIFF_T,
	} length;
	int width;
	char prefix, base, *s;
	char buf[25];
	buf[24] = 0;
	
	for(i=0; (c = *format++); i++) {
		if(c != '%') {
			terminal_putc_simple(c);
			continue;
		}
		length = LENGTH_INT;
		width = 0;
		prefix = 0;
		pad = ' ';
		base = 10;
		
		while(1) {
			switch(c = *format++) {
				case 0:
					goto end;
				case '%':
					terminal_putc_simple(c);
					goto next;
				case '#':
					prefix = 1;
					break;
				case '0':
					if(!width) {
						pad = '0';
						break;
					}
				case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					width = width*10 + (c - '0');
					break;
				case 'h':
					length = length == LENGTH_SHORT ? LENGTH_CHAR : LENGTH_SHORT;
					break;
				case 'l':
					length = length == LENGTH_LONG ? LENGTH_LONG_LONG : LENGTH_LONG;
					break;
				case 'j':
					length = LENGTH_INTMAX_T;
					break;
				case 'z':
					length = LENGTH_SIZE_T;
					break;
				case 't':
					length = LENGTH_PTRDIFF_T;
					break;
				case 'o':
					base = 8;
					if(prefix)
						terminal_putc_simple('0');
					goto baseconv;
				case 'p':
					length = sizeof(void *);
					prefix = 1;
				case 'x':
				case 'X':
					base = 16;
					if(prefix)
						terminal_puts("0x");
				case 'u':
					baseconv:
					switch(length) {
						case LENGTH_CHAR:
							PRINT_TYPE(unsigned char, unsigned int, 0);
							break;
						case LENGTH_SHORT:
							PRINT_TYPE(unsigned short, unsigned int, 0);
							break;
						case LENGTH_INT:
							PRINT_TYPE(unsigned int, unsigned int, 0);
							break;
						case LENGTH_LONG:
							PRINT_TYPE(unsigned long, unsigned long, 0);
							break;
						case LENGTH_LONG_LONG:
							PRINT_TYPE(unsigned long long, unsigned long long, 0);
							break;
						default:
							break;
					}
					goto next;
				case 'd':
				case 'i':
					switch(length) {
						case LENGTH_CHAR:
							PRINT_TYPE(signed char, signed int, 1);
							break;
						case LENGTH_SHORT:
							PRINT_TYPE(signed short, signed int, 1);
							break;
						case LENGTH_INT:
							PRINT_TYPE(signed int, signed int, 1);
							break;
						case LENGTH_LONG:
							PRINT_TYPE(signed long, signed long, 1);
							break;
						case LENGTH_LONG_LONG:
							PRINT_TYPE(signed long long, signed long long, 1);
							break;
						default:
							break;
					}
					goto next;
				case 's':
					terminal_puts(va_arg(va, char *));
					goto next;
				case 'c':
					terminal_putc_simple((char) va_arg(va, int));
					goto next;
			}
		}
		next:;
	}
	end:
	return i;
}

int printf(char *format, ...) {
	int ret;
	va_list va;
	va_start (va, format);
	ret = vprintf(format, va);
	va_end(va);
	return ret;
}
