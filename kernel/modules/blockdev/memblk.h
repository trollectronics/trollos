#ifndef __MEMBLK_H__
#define	__MEMBLK_H__

#define	MAX_MEMBLK		4

struct MemblkDescriptor {
	void			*addr;
	int32_t			pos;
	int32_t			len;
};

#ifndef _MEMBLK
int memblk_init();
int memblk_open(int pid, void *ptr, uint32_t length);
int memblk_seek(int pid, int blk, uint64_t offset);
int memblk_write(int pid, int blk, void *buf, uint32_t count);
int memblk_read(int pid, int blk, void *buf, uint32_t count);
int32_t memblk_blksize(int pid, int id);
#endif


#endif
