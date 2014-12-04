#include "blockdev.h"

struct Blockdev blockdev_state;

#define	BLOCKDEV_CHECK_DEVICE(d)				\
	if ((d) >= BLOCKDEV_MAX)				\
		return BLOCKDEV_STATUS_BAD_DEV;			\
	if (!blockdev_state.entry[(d)].valid)			\
		return BLOCKDEV_STATUS_BAD_DEV;

bool blockdev_init() {
	int i;

	for (i = 0; i < BLOCKDEV_HANDLER_MAX; i++)
		blockdev_state.handler[i].valid = false;
	return true;
}


int blockdev_iface_add(struct BlockdevHandler bd) {
	int i;

	for (i = 0; i < BLOCKDEV_HANDLER_MAX; i++)
		if (!blockdev_state.handler[i].valid) {
			blockdev_state.handler[i] = bd, blockdev_state.handler[i].valid = true;
			return i;
		}
	return BLOCKDEV_STATUS_NOAVAIL;
}


int blockdev_iface_del(int blockdev) {
	if (blockdev >= BLOCKDEV_HANDLER_MAX)
		return BLOCKDEV_STATUS_BAD_DEV;
	if (!blockdev_state.handler[blockdev].valid)
		return BLOCKDEV_STATUS_BAD_DEV;
	if (!blockdev_state.handler[blockdev].unload)
		return BLOCKDEV_STATUS_BUSY;
	return BLOCKDEV_STATUS_OK;
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
