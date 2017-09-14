#ifndef INCLUDE_DEVICE_H_
#define INCLUDE_DEVICE_H_

#include "sys/types.h"

typedef struct CharDev CharDev;
struct CharDev {
	ssize_t (*read)(void *buf, size_t count);
	ssize_t (*write)(const void *buf, size_t count);
	ssize_t (*ioctl)(unsigned long request, ...);
};

typedef struct BlockDev BlockDev;
struct BlockDev {
	ssize_t (*read)(void *buf, size_t count);
	ssize_t (*write)(const void *buf, size_t count);
	ssize_t (*ioctl)(unsigned long request, ...);
};

int device_register_char(const char *name, CharDev *dev);
int device_register_block(const char *name, BlockDev *dev);
#endif
