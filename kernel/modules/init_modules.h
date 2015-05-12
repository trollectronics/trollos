#ifndef __INIT_MODULES_H__
#define	__INIT_MODULES_H__

#include "blockdev/memblk.h"
#include "module.h"

const struct ModuleCall module_init_list[2] = {
	{ "memblk", memblk_init, memblk_open, (void *) 0, memblk_seek, memblk_write, memblk_read, memblk_blksize },
	{ 0, 0, 0, 0, 0 },
};


#endif
