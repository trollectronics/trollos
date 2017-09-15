#ifndef __INIT_MODULES_H__
#define	__INIT_MODULES_H__

#include <stddef.h>
//#include "blockdev/memblk.h"
//#include "blockdev/blkcache.h"
#include "module.h"

const struct ModuleCall const module_init_list[] = {
	#if 0
	{ "blkcache", blkcache_init, NULL, blkcache_open_module, blkcache_seek, NULL, blkcache_read, blkcache_blksize, blkcache_devsize },
	{ "memblk", memblk_init, memblk_open, NULL, memblk_seek, memblk_write, memblk_read, memblk_blksize, memblk_devsize },
	#endif
	{ 0, 0, 0, 0, 0, NULL },
};


#endif
