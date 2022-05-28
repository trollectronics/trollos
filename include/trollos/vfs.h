/*
Copyright (c) 2018 Steven Arnow <s@rdw.se>
'vfs.h' - This file is part of trollos

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/


#ifndef TROLLOS_VFS_H_
#define	TROLLOS_VFS_H_

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define	MAX_GLOBAL_FILES			1024
#define	MAX_MOUNT_POINTS			16
#define	MAX_FILE_SYSTEMS			4

struct VFSFileEntryDevice {
	dev_t					dev;
};


struct VFSFSType {
	int					used;
	char					name[16];
	int					(*_mount)(dev_t dev, ino_t *root_inode);
	int					(*_umount)(int instance);
	int					(*_stat)(int instance, ino_t inode, const char *fname, struct stat *s);
	int					(*_stat_inode)(int instance, ino_t inode, struct stat *s);
	ssize_t					(*_read)(int instance, ino_t inode, void *data, size_t bytes, off_t offset);
	int					(*_read_directory)(int instance, ino_t inode, off_t pos, struct dirent *de);
};


struct VFSFileEntryFS {
	/* TODO: Add file system instance here */
	int					instance;
	ino_t					inode;
};


struct VFSFileEntry {
	bool					is_used;
	bool					is_device;
	bool					read;
	bool					write;
	bool					directory;
	bool					unsupported;
	off_t					pos;
	int					ref;
	mode_t					mode;

	union {
		struct VFSFileEntryDevice	dev;
		struct VFSFileEntryFS		fs;
	} data;
};

int vfs_mount(dev_t dev, const char *path, const char *fs);
int vfs_open(const char *path, int flags);
int vfs_open_device(dev_t device, int flags);
int vfs_read_directory(int fd, struct dirent *de, int dirs);
ssize_t vfs_read(int fd, void *buf, size_t count);
ssize_t vfs_write(int fd, void *buf, size_t count);
off_t vfs_seek(int fd, off_t offset, int whence);
off_t vfs_tell(int fd);
int vfs_stat(const char *path, int *mp_out, struct stat *s);
int vfs_close(int fd);
int vfs_fs_register(struct VFSFSType *fs);

#endif
