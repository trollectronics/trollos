#include "file.h"
#include <errno.h>


static struct FileDescriptor fd_global[MAX_GLOBAL_FILES];


static int fd_alloc() {
	int i;

	for (i = 0; i < MAX_GLOBAL_FILES; i++)
		if (fd_global[i].pos == -1)
			return i;
	return -EMFILE;
}


static void fd_return(int fd) {
	if (fd < -1 || fd >= MAX_GLOBAL_FILES)
		return;
	/* TODO: Call close_descrptor on device */
	fd_global[fd].pos = -1;

	return;
}


int fd_init() {
	int i;

	for (i = 0; i < MAX_GLOBAL_FILES; i++)
		fd_return(i);
	return 0;
}


int fd_open(int uid, const char *path, uint32_t flags) {
	int fd;

	if ((fd = fd_alloc()) < 0)
		return fd;
	fd_return(fd);
	return -ENOENT;
}


int fd_write(int uid, int fd, const void *buf, uint32_t count) {
	return count?-ENOSPC:0;
}


int fd_read(int uid, int fd, void *buf, uint32_t count) {
	return 0;
}
