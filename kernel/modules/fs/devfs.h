#ifndef FS_DEVFS_H_
#define	FS_DEVFS_H_


#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int fs_devfs_instantiate(dev_t dev);
int fs_devfs_stat_inode(ino_t inode, struct stat *s);
int fs_devfs_stat(ino_t inode, const char *fname, struct stat *s);
ssize_t fs_devfs_read(ino_t inode, void *data, size_t bytes, off_t offset);
int fs_devfs_read_directory(ino_t inode, off_t pos, struct dirent *de);
int fs_devfs_write(ino_t inode, void *data, size_t bytes, off_t offset);


#endif
