#ifndef FS_ROMFS_H_
#define	FS_ROMFS_H_

#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

int fs_romfs_stat_inode(int instance, ino_t inode, struct stat *s);
int fs_romfs_stat(int instance, ino_t inode, const char *fname, struct stat *s);
ssize_t fs_romfs_read(int instance, ino_t inode, void *data, size_t bytes, off_t offset);
int fs_romfs_read_directory(int instance, ino_t inode, off_t pos, struct dirent *de);
int fs_romfs_init();
int fs_romfs_mount(dev_t dev, ino_t *root_inode);

#endif
