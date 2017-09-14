#include <device.h>

int device_register(const char *name, Device *device, dev_t *device_number) {
	//allocate device number, add to tree, return device number
	*device_number = 0x10001000;
	return 0;
}
