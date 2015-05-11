#ifndef __DEVICE_H__
#define	__DEVICE_H__

#include <stdbool.h>
#include <stdint.h>
#define	DEVICE_MAX		64

enum DeviceStatus {
	DEVICE_STATUS_NOHANDLER = -8,
	DEVICE_STATUS_BUSY	= -7,
	DEVICE_STATUS_NOSIZE	= -6,
	DEVICE_STATUS_NOAVAIL	= -5,
	DEVICE_STATUS_ARGLEN	= -4,
	DEVICE_STATUS_READONLY	= -3,
	DEVICE_STATUS_RANGE	= -2,
	DEVICE_STATUS_BAD_DEV	= -1,
	DEVICE_STATUS_OK	= 0,
};

enum DeviceType {
	DEVICE_TYPE_BLOCKDEV,
	DEVICE_TYPE_INVALID,
};

struct DeviceEntry {
	bool			valid;
	char			name[16];
	enum DeviceType		type;
	uint32_t		device;
	uint32_t		subdevice;
};

struct Device {
	struct DeviceEntry	device[DEVICE_MAX];
};

extern struct Device device_state;

bool device_init();
int device_add(const char *name, enum DeviceType type, uint32_t device, uint32_t subdevice);
int device_del(uint32_t device);

#endif
