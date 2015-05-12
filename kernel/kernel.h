#ifndef __KERNEL_H__
#define __KERNEL_H__
#include <stdint.h>

typedef uint32_t size_t;
typedef int32_t ssize_t;

void panic(const char *message);

#endif
