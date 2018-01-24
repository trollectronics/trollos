#ifndef INCLUDE_DEVICE_H_
#define INCLUDE_DEVICE_H_

#include "sys/types.h"

typedef enum DeviceType DeviceType;
enum DeviceType {
	DEVICE_TYPE_INVALID = -1,
	DEVICE_TYPE_CHAR,
	DEVICE_TYPE_BLOCK,
};

typedef struct CharDev CharDev;
struct CharDev {
	ssize_t (*read)(void *buf, size_t count);
	ssize_t (*write)(const void *buf, size_t count);
	ssize_t (*ioctl)(unsigned long request, ...);
};

typedef struct BlockDev BlockDev;
struct BlockDev {
	ssize_t (*read)(dev_t device, off_t pos, void *buf, size_t count);
	ssize_t (*write)(const void *buf, size_t count);
	ssize_t (*size)(dev_t device);
	ssize_t (*blksize)(dev_t device);
	ssize_t (*ioctl)(unsigned long request, ...);
};

typedef struct Device Device;
struct Device {
	DeviceType type;
	union {
		CharDev chardev;
		BlockDev blockdev;
	};
};

int device_register(const char *name, Device *device, dev_t *device_number);
Device *device_lookup(dev_t device_number);
dev_t device_lookup_name(const char *name, Device **device);

#endif
