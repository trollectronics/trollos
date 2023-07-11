#ifndef INCLUDE_DEVICE_H_
#define INCLUDE_DEVICE_H_

#define	DEVICE_NAME_LEN 32

#include "sys/types.h"


typedef struct DeviceIOQueue DeviceIOQueue;
struct DeviceIOQueue {
	pid_t pid;

	DeviceIOQueue *next;
};

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

	DeviceIOQueue *queue_read;
	DeviceIOQueue *queue_write;
	DeviceIOQueue *queue_ioctl;
};

typedef struct BlockDev BlockDev;
struct BlockDev {
	ssize_t (*read)(dev_t device, off_t pos, void *buf, size_t count);
	ssize_t (*write)(const void *buf, size_t count);
	ssize_t (*size)(dev_t device);
	ssize_t (*blksize)(dev_t device);
	ssize_t (*ioctl)(unsigned long request, ...);

	DeviceIOQueue *queue;
};

typedef struct Device Device;
struct Device {
	DeviceType type;
	union {
		CharDev chardev;
		BlockDev blockdev;
	};
	void *priv;
};


typedef struct DeviceRegistration DeviceRegistration;
struct DeviceRegistration {
	char name[DEVICE_NAME_LEN];
	dev_t device;
	DeviceType type;
};

int device_register(const char *name, Device *device, dev_t *device_number_out);
Device *device_lookup(dev_t device_number);
dev_t device_lookup_name(const char *name, Device **device);
int device_list(DeviceRegistration *dr, int id);
int device_max();

ssize_t chardev_read(Device *dev, void *buf, size_t count);
ssize_t chardev_write(Device *dev, const void *buf, size_t count);
ssize_t chardev_ioctl(Device *dev, unsigned long request, ...);

#endif
