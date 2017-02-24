#ifndef INCLUDE_DEVICE_H_
#define INCLUDE_DEVICE_H_

#include "sys/types.h"

typedef struct CharDev CharDev;
struct CharDev {
	ssize_t (*read)(void *buf, size_t count);
	ssize_t (*write)(const void *buf, size_t count);
	ssize_t (*ioctl)(unsigned long request, ...);
};

#endif
