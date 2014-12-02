#ifndef __ROMFS_H__
#define	__ROMFS_H__

#define	ROMFS_MAGIC1		0x2D726F6D
#define	ROMFS_MAGIC2		0x3166732D

/* http://lxr.free-electrons.com/source/Documentation/filesystems/romfs.txt */

struct RomfsMainStruct {
	unsigned int		magic1;
	unsigned int		magic2;
	unsigned int		fs_size;
	unsigned int		checksum;
	/* Volume name follows */
};


struct RomfsFileEntry {
	unsigned int		next_fileheader;
	unsigned int		special_info;
	unsigned int		size;
	unsigned int		checksum;
	/* File name and data follows */
};


struct RomfsFileDescriptor {
	const char		*filename;
	unsigned int		size;
	void			*data;
};


int romfs_detect(void *ptr);
struct RomfsFileDescriptor romfs_locate(void *ptr, char *fname);

#endif
