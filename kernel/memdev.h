#ifndef __MEMDEV_H__
#define	__MEMDEV_H__

#include <stdbool.h>
#include <stdint.h>

#define	MEMDEV_MAX		8

struct MemdevEntry {
	uint32_t		*ptr;
	uint32_t		size;
	uint32_t		block_size;
	uint32_t		blockdev;
	bool			writable;
};


struct Memdev {
	struct MemdevEntry	entry[MEMDEV_MAX];
	int			iface;
};

int memdev_from_arg(const char *opt);
int memdev_read(uint32_t device, uint32_t block, uint32_t count, uint32_t *dest);
int memdev_write(uint32_t device, uint32_t block, uint32_t count, uint32_t *data);

extern struct Memdev memdev_state;

#endif
