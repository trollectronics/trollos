#ifndef __ROMFS_H__
#define	__ROMFS_H__

#define	ROMFS_MAGIC1		0x2D726F6D
#define	ROMFS_MAGIC2		0x3166732D
#define	MAX_ROMFS		2

/* http://lxr.free-electrons.com/source/Documentation/filesystems/romfs.txt */
#include <stdint.h>
#include <stdbool.h>

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
	struct {
		int32_t		major;
		int		minor;
	} blkcache;

	uint32_t		inode_offset;
};


int romfs_detect(void *ptr);
struct RomfsFileDescriptor romfs_locate(void *ptr, char *fname);

#endif
