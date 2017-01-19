#ifndef __MEMBLK_H__
#define	__MEMBLK_H__

#define	MAX_MEMBLK		4

#include <sys/types.h>

struct MemblkDescriptor {
	void			*addr;
	int32_t			pos;
	int32_t			len;
};

#ifndef _MEMBLK
int memblk_init();
int memblk_open(int pid, void *ptr, uint32_t length);
off_t memblk_seek(int pid, int blk, off_t offset, uint32_t whence);
int memblk_write(int pid, int blk, void *buf, uint32_t count);
int memblk_read(int pid, int blk, void *buf, uint32_t count);
int32_t memblk_blksize(int pid, int id);
#endif


#endif
