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
};

#endif
