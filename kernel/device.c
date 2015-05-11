#include "device.h"

struct Device device_state;

bool device_init() {
	int i;

	for (i = 0; i < DEVICE_MAX; i++)
		device_state.device[i].valid = false;
	return true;
}


int device_add(const char *name, enum DeviceType type, uint32_t device, uint32_t subdevice) {
	int i;

	for (i = 0; i < DEVICE_MAX; i++)
		if (!device_state.device[i].valid)
			break;
	if (i == DEVICE_MAX)
		return DEVICE_STATUS_NOAVAIL;
	device_state.device[i].valid = true;
	strncpy(device_state.device[i].name, name, 16);
	device_state.device[i].type = type;
	device_state.device[i].subdevice = subdevice;
	return i;
}


int device_del(uint32_t device) {
	int i;

	if (device >= DEVICE_MAX)
		return DEVICE_STATUS_BAD_DEV;
	device_state.device[device].valid = false;
	return DEVICE_STATUS_OK;
}
