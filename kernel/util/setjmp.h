#ifndef __UTIL_SETJMP_H__
#define __UTIL_SETJMP_H__
#include <stdint.h>

typedef struct {
	uint32_t x[16];
} jmp_buf[1];

int setjmp(jmp_buf env);
int longjmp(jmp_buf env, int value);

#endif
