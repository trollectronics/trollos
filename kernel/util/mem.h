#ifndef __MEM_H__
#define __MEM_H__

#include <stdint.h>
#include "../mmu.h"

void *ksbrk(intptr_t increment);
void *kmalloc(uint32_t size);
void kfree(void *pointer);

void *memset(void *pointer, int c, uint32_t n);
void *memcpy(void *dest, void *src, uint32_t n);

void *memset_user(void *pointer, int c, uint32_t n);
#define memcpy_from_user mmu040_copy_from_userspace
#define memcpy_to_user mmu040_copy_to_userspace


#endif
