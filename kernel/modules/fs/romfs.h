#ifndef FS_ROMFS_H_
#define	FS_ROMFS_H_

#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

int fs_romfs_instantiate(dev_t dev);
int fs_romfs_stat_inode(ino_t inode, struct stat *s);
int fs_romfs_stat(ino_t inode, const char *fname, struct stat *s);
ssize_t fs_romfs_read(ino_t inode, void *data, size_t bytes, off_t offset);
int fs_romfs_read_directory(ino_t inode, off_t pos, struct dirent *de);

#endif
