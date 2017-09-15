#include "blkcache.h"
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include "../../util/log.h"
#include "../../util/mem.h"
#include <sys/types.h>
#include <module.h>
#include <device.h>

// It's early days, so it's okay to be stupid //

struct BlkCacheEntry romfs_blkcache_open(uint32_t major, uint32_t minor) {
	struct BlkCacheEntry blk;
	int b;
	
	blk.offset = 0;
	blk.major = major;
	blk.minor = minor;

	return blk;
}


off_t romfs_blkcache_seek(struct BlkCacheEntry *blk, off_t offset, uint32_t whence) {
	int64_t devsize;
	dev_t devno = makedev(blk->major, blk->minor);
	struct Device *dev = device_lookup(devno);
	if (!dev)
		return -ENODEV;
	if (dev->type != DEVICE_TYPE_BLOCK)
		return -ENODEV;

	if (!dev->blockdev.size)
		return -EPERM;
	
	devsize = dev->blockdev.size(devno);
		

	if (whence == SEEK_SET)
		blk->offset = offset;
	if (whence == SEEK_CUR)
		blk->offset += offset;
	if (whence == SEEK_END)
		blk->offset = devsize + offset;
	if (blk->offset < 0)
		blk->offset = 0;
	if (blk->offset > devsize)
		blk->offset = devsize;

	return blk->offset;
}


off_t romfs_blkcache_read(struct BlkCacheEntry *blk, void *ptr, off_t count) {
	int ret;
	int64_t pos, i;
	uint32_t copied, blksz;
	dev_t devno = makedev(blk->major, blk->minor);
	struct Device *dev = device_lookup(devno);

	if (!dev)
		return -ENODEV;
	if (dev->type != DEVICE_TYPE_BLOCK)
		return -ENODEV;
	if (!dev->blockdev.blksize)
		return -EPERM;
	if (!dev->blockdev.read)
		return -EPERM;
	
	blksz = dev->blockdev.blksize(devno);
	pos = blk->offset;

	if (pos + count >= dev->blockdev.size(devno))
		count = dev->blockdev.size(devno) - pos;
	if (count <= 0)
		return 0;
	
	{
		uint8_t buff[blksz];
		uint32_t data_count;
		
		for (copied = 0, i = (pos & (~(((int64_t) blksz) - 1))); copied < count; copied += data_count) {
			data_count = (count - copied);
			if (pos & (blksz - 1))
				data_count = (pos & (blksz - 1));
			if ((ret = dev->blockdev.read(devno, pos, buff, blksz)) <= 0)
				return ret;
			memcpy(ptr + copied, buff + (blksz - data_count), data_count);
		}
	}

	return copied;
}
