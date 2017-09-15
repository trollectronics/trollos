#ifndef __FILE_H__
#define	__FILE_H__

#include <stdint.h>

#define	MAX_GLOBAL_FILES		8

struct FileDescriptor {
	/* Instance of <file type> in driver */
	uint16_t			major;
	uint32_t			minor;

	uint32_t			flags;
	int64_t				pos;
	int64_t				ino;
	int				size;
	uint32_t			ref;
};

extern struct FileDescriptor vfs_file_descriptor[MAX_GLOBAL_FILES];

int fd_write(int uid, int fd, const void *buf, uint32_t count);

#endif
