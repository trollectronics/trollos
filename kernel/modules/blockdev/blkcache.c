#include "blkcache.h"
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include "../../util/log.h"
#include "../../util/mem.h"
#include "../module.h"

#define	MAX_BLKCACHE		4

// It's early days, so it's okay to be stupid //

struct BlkCacheEntry {
	off_t			offset;
	int			major;
	int			minor;
	bool			used;
};


static struct BlkCacheEntry blk[MAX_BLKCACHE];


int blkcache_init() {
	int i;

	for (i = 0; i < MAX_BLKCACHE; i++)
		blk[i].used = false;
	kprintf(LOG_LEVEL_INFO, "[blkcache] Module initialized\n");

	return 0;
}


static int _alloc() {
	int i;

	for (i = 0; i < MAX_BLKCACHE; i++)
		if (!blk[i].used)
			return blk[i].used = true, i;
	return -EMFILE;
}


int blkcache_open_module(void *aux, uint32_t major, uint32_t minor, uint32_t flags) {
	int b;
	
	if ((b = _alloc()) < 0)
		return b;
	blk[b].offset = 0;
	blk[b].major = major;
	blk[b].minor = minor;

	return b;
}


off_t blkcache_seek(int context, off_t offset, uint32_t whence) {
	int64_t devsize;

	devsize = module_devsize(blk[context].major, blk[context].minor);

	if (whence == SEEK_SET)
		blk[context].offset = offset;
	if (whence == SEEK_CUR)
		blk[context].offset += offset;
	if (whence == SEEK_END)
		blk[context].offset = devsize + offset;
	if (blk[context].offset < 0)
		blk[context].offset = 0;
	if (blk[context].offset > devsize)
		blk[context].offset = devsize;

	return blk[context].offset;
}


off_t blkcache_read(int context, void *ptr, off_t count) {
	int ret;
	int64_t pos, i;
	uint32_t copied, blksz;

	blksz = module_blksize(blk[context].major, blk[context].minor);
	pos = blk[context].offset;
	
	if (pos + count >= module_devsize(blk[context].major, blk[context].minor))
		count = module_devsize(blk[context].major, blk[context].minor) - pos;
	if (!count)
		return 0;
	
	{
		uint8_t buff[blksz];
		uint32_t data_count;
		
		for (copied = 0, i = (pos & (~(((int64_t) blksz) - 1))); copied < count; copied += data_count) {
			data_count = (count - copied);
			if (pos & (blksz - 1))
				data_count = (pos & (blksz - 1));
			if ((ret = module_seek(blk[context].major, blk[context].minor, i, SEEK_SET)) < 0)
				return ret;
			if ((ret = module_read(blk[context].major, blk[context].minor, buff, blksz)) <= 0)
				return ret;
			memcpy(ptr + copied, buff + (blksz - data_count), data_count);
		}
	}

	return copied;
}


int32_t blkcache_blksize(int minor) {
	return module_blksize(blk[minor].major, blk[minor].minor);
}


int64_t blkcache_devsize(int minor) {
	return module_devsize(blk[minor].major, blk[minor].minor);
}
