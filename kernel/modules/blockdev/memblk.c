#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include "../../util/mem.h"
#include "../../util/log.h"

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


int memblk_open(void *ptr, uint32_t length) {
	int d;

	if (length >= INT_MAX)
		return -EINVAL;
	d = memblk_alloc();
	memblk[d].len = length;
	memblk[d].addr = ptr;
	
	/* TODO: Announce new memblk */

	return d;
}


off_t memblk_seek(int blk, off_t offset, uint32_t whence) {
	if (blk < 0 || blk >= MAX_MEMBLK)
		return -EINVAL;
	if (memblk[blk].len < 0)
		return -EBADF;
	if (whence != SEEK_SET)
		return -EINVAL;
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


off_t memblk_read(int blk, void *buf, off_t count) {
	if (blk < 0 || blk >= MAX_MEMBLK)
		return -EINVAL;
	if (memblk[blk].len < 0)
		return -EBADF;
	if (((off_t) memblk[blk].pos) + count < memblk[blk].pos || ((off_t) memblk[blk].pos + count > memblk[blk].len))
		return -EIO;
	memcpy(buf, memblk[blk].addr + memblk[blk].pos, count);

	return count;
}


int32_t memblk_blksize(int blk) {
	return 512;
}


int64_t memblk_devsize(int blk) {
	return memblk[blk].len;
}
