#ifndef __BLKCACHE_H__
#define	__BLKCACHE_H__

#include <sys/types.h>

int blkcache_init();
int blkcache_open_module(void *aux, uint32_t major, uint32_t minor, uint32_t flags);
off_t blkcache_seek(int context, off_t offset, uint32_t whence);
off_t blkcache_read(int context, void *ptr, off_t count);
int32_t blkcache_blksize(int minor);
int64_t blkcache_devsize(int minor);


#endif
