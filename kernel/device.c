#include <device.h>
#include <errno.h>
#include "util/string.h"
#include "util/log.h"
#include "util/mem.h"

#define DEVICE_MAX 256

typedef struct RegisteredDevice RegisteredDevice;
struct RegisteredDevice {
	char name[DEVICE_NAME_LEN];
	Device *device;
};

//TODO: use proper arraylist
static RegisteredDevice *_device_registered[DEVICE_MAX];
static uint16_t major_free = 1;

int device_register(const char *name, Device *device, dev_t *device_number_out) {
	int i;
	
	//TODO: support minor numbers
	dev_t devno = (major_free << 16) | 0x1;
	*device_number_out = devno;
	
	for(i = 0; i < DEVICE_MAX; i++) {
		if(!_device_registered[major(devno)])
			continue;
		if(!strcmp(_device_registered[i]->name, name)) {
			kprintf(LOG_LEVEL_ERROR, "Device name '%s' already taken\n", name);
			return -1;
		}
		
		if(device == _device_registered[i]->device) {
			kprintf(LOG_LEVEL_ERROR, "Device '%s' already registered as '%s'\n", name, _device_registered[i]->name);
			return -1;
		}	
	}
	
	RegisteredDevice *dev = NULL;
	
	if(!(dev = kmalloc(sizeof(RegisteredDevice)))) {
		return -ENOMEM;
	}
	
	strcpy(dev->name, name);
	dev->device = device;
	
	_device_registered[major_free] = dev;
	major_free++;
	
	return 0;
}

Device *device_lookup(dev_t device_number) {
	if(!_device_registered[major(device_number)])
		return NULL;
	return _device_registered[major(device_number)]->device;
}


// TODO: Fix, will probably crash or malfunction if an invalid or free entry is encountered
dev_t device_lookup_name(const char *name, Device **device) {
	uint32_t i;
	for (i = 0; i < DEVICE_MAX; i++)
		if (_device_registered[i])
			if (!strcmp(_device_registered[i]->name, name)) {
				if(device)
					*device = _device_registered[i]->device;
				
				return makedev(i, 1);
			}
	
	return 0;
}


int device_list(DeviceRegistration *dr, int id) {
	if (id < DEVICE_MAX)
		return -ENOENT;
	if (_device_registered[id]) {
		strncpy(dr->name, _device_registered[id]->name, DEVICE_NAME_LEN);
		dr->name[DEVICE_NAME_LEN-1] = 0;
		dr->device = makedev(id, 1);
		dr->type = _device_registered[id]->device->type;
		return dr;
	}

	return -ENOENT;
}


int device_max() {
	return DEVICE_MAX;
}
