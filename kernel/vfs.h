#ifndef __VFS_H__
#define	__VFS_H__

#include <sys/types.h>
#include <limits.h>
#include <stdint.h>

#define	MOUNT_PATH_MAX		64
#define	VFS_MOUNT_MAX		16
#define	VFS_FOLLOW_MAX		32

enum VfsFsType {
	VFS_FS_TYPE_INVALID = -1,
	VFS_FS_TYPE_ROMFS,
};


enum VfsFileType {
	S_IFIFO			= 0010000,
	S_IFCHR			= 0020000,
	S_IFDIR			= 0040000,
	S_IFBLK			= 0060000,
	S_IFREG			= 0100000,
	S_IFLNK			= 0120000,
	S_IFSOCK		= 0140000,
	S_IFMT			= 0170000,
};


struct VfsFile {
	uint64_t		inode;
	uint32_t		vfs;
	char			filename[256];
	uint16_t		permissions;
};

struct VfsInode {
	int			mount;
	int			context_id;
	int64_t			inode;
};



struct VfsMount {
	uint64_t		mount_inode;
	uint32_t		mount_vfs;
	char			mount_path[MOUNT_PATH_MAX];
	enum VfsFsType		fs_type;

	struct {
		uint32_t	read	: 1;
		uint32_t	write	: 1;
	} flags;

	struct VfsInode		mounted_on;
	struct VfsInode		root;
};


struct Vfs {
	struct VfsInode		root;
	struct VfsMount		mount[VFS_MOUNT_MAX];
};

extern struct Vfs vfs_state;

#endif
