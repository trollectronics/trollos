#ifndef __ROMFS_H__
#define	__ROMFS_H__

#define	MAX_ROMFS		2

/* http://lxr.free-electrons.com/source/Documentation/filesystems/romfs.txt */
#include <stdint.h>
#include <stdbool.h>
#include "blkcache.h"

struct RomfsMainStruct {
	uint32_t		magic1;
	uint32_t		magic2;
	uint32_t		fs_size;
	uint32_t		checksum;
	/* Volume name follows */
};


struct RomfsFileEntry {
	uint32_t		next_fileheader;
	uint32_t		special_info;
	uint32_t		size;
	uint32_t		checksum;
	/* File name and data follows */
};


struct RomfsFileDescriptor {
	const char		*filename;
	uint32_t		size;
	void			*data;
};


struct RomfsDescriptor {
	struct BlkCacheEntry blkcache;

	uint32_t		inode_offset;
};


int romfs_init();
int romfs_open_device(void *aux, uint32_t major, uint32_t minor, uint32_t flags, uint64_t *root_inode);
int romfs_inode_stat(int context, int64_t inode, const char *name, struct stat *st_s);
int romfs_read(int fs, int64_t inode, off_t offset, void *buf, uint32_t count);


#endif
