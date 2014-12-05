#ifndef __VFS_H__
#define	__VFS_H__

#include <stdint.h>

#define	MOUNT_PATH_MAX		64
#define	VFS_MAX_MOUNT		16

enum VfsFsType {
	VFS_FS_TYPE_INAVLID,
	VFS_FS_TYPE_ROMFS,
};


struct VfsFile {
	uint64_t		inode;
	uint32_t		vfs;
	char			filename[256];
	uint16_t		permissions;
};


struct VfsMount {
	uint64_t		mount_inode
	uint32_t		mount_vfs;
	char			mount_path[MOUNT_PATH_MAX];
	enum VfsFsType		fs_type;

	struct {
		uint32_t	read	: 1;
		uint32_t	write;	: 1;
	} flags;

	void			*vfs_handle;
};


struct Vfs {
	VfsMount		mount[VFS_MAX_MOUNT];
};

struct VfsHandler {
}

extern struct Vfs vfs_state;

#endif
