#ifndef __MEMBLK_H__
#define	__MEMBLK_H__

#define	MAX_MEMBLK		4

#include <sys/types.h>

struct MemblkDescriptor {
	void			*addr;
	off_t			pos;
	int64_t			len;
	dev_t			device;
};

#ifndef _MEMBLK
int memblk_init();
int memblk_open(void *ptr, uint32_t length);
#endif


#endif
