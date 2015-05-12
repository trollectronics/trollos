#include "log.h"
#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include "mem.h"

#define	_MEMBLK
#include "memblk.h"

static struct MemblkDescriptor memblk[MAX_MEMBLK];

static int memblk_alloc() {
	int i;

	for (i = 0; i < MAX_MEMBLK; i++)
		if (memblk[i].len < 0)
			return -ENOENT;
	return i;
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


int memblk_open(int uid, void *ptr, uint32_t length) {
	int d;

	if (length >= INT_MAX)
		return -EINVAL;
	d = memblk_alloc();
	memblk[d].len = length;
	memblk[d].addr = ptr;
	
	/* TODO: Announce new memblk */

	return d;
}


int64_t memblk_seek(int uid, int blk, int64_t offset) {
	if (uid < 0 || uid >= MAX_MEMBLK)
		return -EINVAL;
	if (memblk[blk].len < 0)
		return -EBADF;
	if (offset < 0 || (offset & 0x1FF))
		return -EINVAL;
	if (offset > memblk[blk].len)
		return -EINVAL;
	memblk[blk].pos = offset;
	return memblk[blk].pos;
}


int memblk_write(int uid, int blk, void *buf, uint32_t count) {
	return -EBADF;
}


int memblk_read(int uid, int blk, void *buf, uint32_t count) {
	if (blk < 0 || blk >= MAX_MEMBLK)
		return -EINVAL;
	if (memblk[blk].len < 0)
		return -EBADF;
	if (((uint32_t) memblk[blk].pos) + count < memblk[blk].pos || ((uint32_t) memblk[blk].pos + count > memblk[blk].len))
		return -EIO;
	if (uid)
		memcpy_to_user(buf, memblk[blk].addr + memblk[blk].pos, count);
	else
		memcpy(buf, memblk[blk].addr + memblk[blk].pos, count);

	return count;
}


int32_t memblk_blksize(int uid, int id) {
	return 512;
}
