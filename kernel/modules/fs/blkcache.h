#ifndef __BLKCACHE_H__
#define	__BLKCACHE_H__

#include <stdint.h>
#include <sys/types.h>

struct BlkCacheEntry {
	off_t			offset;
	int			major;
	int			minor;
};

off_t romfs_blkcache_seek(struct BlkCacheEntry *blk, off_t offset, uint32_t whence);
off_t romfs_blkcache_read(struct BlkCacheEntry *blk, void *ptr, off_t count);
struct BlkCacheEntry romfs_blkcache_open(uint32_t major, uint32_t minor);


#endif
