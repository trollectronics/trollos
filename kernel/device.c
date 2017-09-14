#include <device.h>
#include <errno.h>
#include "util/string.h"
#include "util/log.h"
#include "util/mem.h"

#define DEVICE_MAX 256

typedef struct RegisteredDevice RegisteredDevice;
struct RegisteredDevice {
	char name[32];
	Device *device;
};

//TODO: use proper arraylist
static RegisteredDevice *_device_registered[DEVICE_MAX];
static uint16_t major_free = 1;

int device_register(const char *name, Device *device, dev_t *device_number) {
	int i;
	
	//TODO: support minor numbers
	*device_number = (major_free << 16) | 0x1;
	
	for(i = 0; i < DEVICE_MAX; i++) {
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
