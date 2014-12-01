#include <stdarg.h>
#include "terminal.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define PRINT_TYPE(type, argtype) \
s = int_to_string((type) va_arg(va, argtype), buf + 24, base); \
j = 24 + buf - s; \
if(!width) \
	width = j; \
else \
	while(width > j)  {\
		terminal_putc_simple(pad); \
		width--; \
	} \
terminal_put_counted(s, width)



static char *int_to_string(unsigned long long int n, char *s, int base) {
	char i;
	do {
		s--;
		i = n  % base;
		i += (i > 9 ? 'A' - 10 : '0');
		*s = i;
		n /= base;
	} while(n);
	return s;
}

int printf(char *format, ...) {
	//TODO: handle signed values
	unsigned char pad, c;
	int i;
	unsigned int j;
	va_list va;
	
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
	
	va_start(va, format);
	for(i=0; (c = *format++); i++) {
		if(c != '%') {
			terminal_putc_simple(c);
			continue;
		}
		length = LENGTH_INT;
		width = 0;
		prefix = 0;
		pad = ' ';
		
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
					goto baseconv;
				case 'u':
				case 'd':
				case 'i':
					base = 10;
					baseconv:
					switch(length) {
						case LENGTH_CHAR:
							PRINT_TYPE(unsigned char, unsigned int);
							break;
						case LENGTH_SHORT:
							PRINT_TYPE(unsigned short, unsigned int);
							break;
						case LENGTH_INT:
							PRINT_TYPE(unsigned int, unsigned int);
							break;
						case LENGTH_LONG:
							PRINT_TYPE(unsigned long, unsigned long);
							break;
						case LENGTH_LONG_LONG:
							PRINT_TYPE(unsigned long long, unsigned long long);
							break;
						default:
							break;
					}
					goto next;
				case 's':
					terminal_puts(va_arg(va, char *));
					goto next;
			}
		}
		next:;
	}
	end:
	va_end(va);
	return i;
}

