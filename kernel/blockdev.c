#include "string.h"
#include "blockdev.h"
#include "device.h"

struct Blockdev blockdev_state;

#define	BLOCKDEV_CHECK_DEVICE(d)				\
	if ((d) >= BLOCKDEV_MAX)				\
		return DEVICE_STATUS_BAD_DEV;			\
	if (!blockdev_state.entry[(d)].valid)			\
		return DEVICE_STATUS_BAD_DEV;

bool blockdev_init() {
	int i;

	for (i = 0; i < BLOCKDEV_HANDLER_MAX; i++)
		blockdev_state.handler[i].valid = false;
	for (i = 0; i < BLOCKDEV_MAX; i++)
		blockdev_state.entry[i].valid = false;
	return true;
}


int blockdev_iface_add(struct BlockdevHandler bd) {
	int i;

	for (i = 0; i < BLOCKDEV_HANDLER_MAX; i++)
		if (!blockdev_state.handler[i].valid) {
			blockdev_state.handler[i] = bd, blockdev_state.handler[i].valid = true;
			return i;
		}
	return DEVICE_STATUS_NOAVAIL;
}


int blockdev_iface_del(uint32_t blockdev) {
	if (blockdev >= BLOCKDEV_HANDLER_MAX)
		return DEVICE_STATUS_BAD_DEV;
	if (!blockdev_state.handler[blockdev].valid)
		return DEVICE_STATUS_BAD_DEV;
	if (!blockdev_state.handler[blockdev].unload)
		return DEVICE_STATUS_BUSY;
	return DEVICE_STATUS_OK;
}


int blockdev_add(const char *name, uint32_t device, uint32_t subdevice) {
	int i;

	for (i = 0; i < BLOCKDEV_MAX; i++)
		if (!blockdev_state.entry[i].valid)
			break;
	if (i == BLOCKDEV_MAX)
		return DEVICE_STATUS_NOAVAIL;
	blockdev_state.entry[i].valid = true;
	strncpy(blockdev_state.entry[i].name, name, 16);
	blockdev_state.entry[i].handler = device;
	blockdev_state.entry[i].device = subdevice;
	if ((blockdev_state.entry[i].dev = device_add(blockdev_state.entry[i].name, DEVICE_TYPE_BLOCKDEV, device, subdevice)) < 0) {
		blockdev_state.entry[i].valid = false;
		return DEVICE_STATUS_NOAVAIL;
	}

	return i;
}


int blockdev_del(uint32_t entry) {
	BLOCKDEV_CHECK_DEVICE(entry);
	blockdev_state.entry[entry].valid = false;
	device_del(blockdev_state.entry[entry].dev);
	return DEVICE_STATUS_OK;
}


int blockdev_read(uint32_t device, uint32_t block, uint32_t count, uint32_t *data) {
	uint32_t handler, dev;
	
	BLOCKDEV_CHECK_DEVICE(device);
	handler = blockdev_state.entry[device].handler;
	dev = blockdev_state.entry[device].device;
	return blockdev_state.handler[handler].read(dev, block, count, data);
}


int blockdev_write(uint32_t device, uint32_t block, uint32_t count, uint32_t *data) {
	uint32_t handler, dev;

	BLOCKDEV_CHECK_DEVICE(device);
	handler = blockdev_state.entry[device].device;
	dev = blockdev_state.entry[device].device;
	return blockdev_state.handler[handler].write(dev, block, count, data);
}


int blockdev_blocksize(uint32_t device) {
	uint32_t handler, dev;

	BLOCKDEV_CHECK_DEVICE(device);
	handler = blockdev_state.entry[device].device;
	dev = blockdev_state.entry[device].device;
	return blockdev_state.handler[handler].blocksize(dev);
}
