#include "file.h"
#include <errno.h>
#include "../../util/mem.h"
#include "../../util/kconsole.h"

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
	uint8_t buff[512];
	int i, j, cnt;

	for (j = 0; (j<<9) < count; j++) {
		cnt = (count-(j<<9)>512)?512:(count-(j<<9));
		memcpy_from_user(buff, buf, cnt);
		kconsole_write(buff, cnt);
	}

	return count;
}


int fd_read(int uid, int fd, void *buf, uint32_t count) {
	return 0;
}
