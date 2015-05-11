#ifndef __BLOCKDEV_H__
#define	__BLOCKDEV_H__

#include <stdbool.h>
#include <stdint.h>

#define	BLOCKDEV_HANDLER_MAX		8
#define BLOCKDEV_MAX			16

enum BlockdevStatus {
	BLOCKDEV_STATUS_NOHANDLER = -8,
	BLOCKDEV_STATUS_BUSY = -7,
	BLOCKDEV_STATUS_NOSIZE = -6,
	BLOCKDEV_STATUS_NOAVAIL = -5,
	BLOCKDEV_STATUS_ARGLEN = -4,
	BLOCKDEV_STATUS_READONLY = -3,
	BLOCKDEV_STATUS_RANGE = -2,
	BLOCKDEV_STATUS_BAD_DEV = -1,
	BLOCKDEV_STATUS_OK	= 0,
};


struct BlockdevHandler {
	bool			valid;
	int			(*create_from_arg)(const char *arg);
	int			(*read)(uint32_t device, uint32_t block, uint32_t count, uint32_t *data);
	int			(*write)(uint32_t device, uint32_t block, uint32_t count, uint32_t *data);
	bool			(*unload)(void);
	int			(*blocksize)(uint32_t device);
	char			name[32];
};


struct BlockdevEntry {
	bool			valid;
	char			name[16];
	uint32_t		handler;
	uint32_t		device;
	int			dev;
};

struct Blockdev {
	struct BlockdevEntry	entry[BLOCKDEV_MAX];
	struct BlockdevHandler	handler[BLOCKDEV_HANDLER_MAX];
};

bool blockdev_init();
int blockdev_iface_add(struct BlockdevHandler bd);
int blockdev_iface_del(uint32_t blockdev);
int blockdev_add(const char *name, uint32_t device, uint32_t subdevice);
int blockdev_del(uint32_t entry);

#endif
