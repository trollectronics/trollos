#ifndef __MEMBLK_H__
#define	__MEMBLK_H__

#define	MAX_MEMBLK		4

#include <sys/types.h>

struct MemblkDescriptor {
	void			*addr;
	off_t			pos;
	int64_t			len;
};

#ifndef _MEMBLK
int memblk_init();
int memblk_open(void *ptr, uint32_t length);
off_t memblk_seek(int blk, off_t offset, uint32_t whence);
int memblk_write(int blk, void *buf, uint32_t count);
off_t memblk_read(int blk, void *buf, off_t count);
int32_t memblk_blksize(int blk);
int64_t memblk_devsize(int blk);
#endif


#endif
