#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <device.h>
#include <sys/types.h>
#include "../../util/mem.h"
#include "../../util/log.h"

#define	_MEMBLK
#include "memblk.h"

static struct MemblkDescriptor memblk[MAX_MEMBLK];

static int locate_device(dev_t device) {
	int i;

	for (i = 0; i < MAX_MEMBLK; i++) {
		if (memblk[i].len >= 0)
			if (memblk[i].device == device)
				return i;
	}
	return -ENOENT;
}


static int memblk_alloc() {
	int i;

	for (i = 0; i < MAX_MEMBLK; i++)
		if (memblk[i].len < 0)
			return i;
	return -ENOENT;
}


static void memblk_return(int d) {
	if (d < 0 || d >= MAX_MEMBLK)
		return;
	memblk[d].len = -1;
}


int memblk_init() {
	int i;
	
	for (i = 0; i < MAX_MEMBLK; i++)
		memblk_return(i);
	
	kprintf(LOG_LEVEL_INFO, "[memblk] Module initialized\n");
	return 0;
}


int memblk_write(int uid, int blk, void *buf, uint32_t count) {
	return -EBADF;
}


static ssize_t memblk_read(dev_t device, off_t pos, void *buf, size_t count) {
	int blk = locate_device(device);
	
	if (blk < 0)
		return blk;
	
	if (memblk[blk].len < pos + count)
		return -EIO;
	if (pos < 0)
		return -EIO;
	memcpy(buf, memblk[blk].addr + pos, count);

	return count;
}


static ssize_t memblk_blksize(dev_t device) {
	return 512;
}


static ssize_t memblk_devsize(dev_t device) {
	int blk = locate_device(device);

	if (blk < 0)
		return blk;
	return memblk[blk].len;
}

static struct Device dev = { 0 };

int memblk_open(void *ptr, uint32_t length) {
	int d, ret;

	if (length >= INT_MAX)
		return -EINVAL;
	d = memblk_alloc();
	memblk[d].len = length;
	memblk[d].addr = ptr;

	dev.type = DEVICE_TYPE_BLOCK;
	dev.blockdev.read = memblk_read;
	dev.blockdev.blksize = memblk_blksize;
	dev.blockdev.size = memblk_devsize;

	ret = device_register("memblk0", &dev, &memblk[d].device);
	if (ret < 0)
		kprintf(LOG_LEVEL_ERROR, "[memblk] Failed to register memblk0\n");
	else
		kprintf(LOG_LEVEL_INFO, "[memblk] Registered memblk0 at %i, %i [@0x%X]\n", major(memblk[d].device), minor(memblk[d].device), &dev);
	return ret;
}
