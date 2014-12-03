#ifndef __MEM_H__
#define __MEM_H__

#include <stdint.h>

void *malloc(uint32_t size);
void free(void *pointer);

void *memset(void *pointer, int c, uint32_t n);
void *memcpy(void *dest, void *src, uint32_t n);

#endif
