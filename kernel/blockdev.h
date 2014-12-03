#ifndef __BLOCKDEV_H__
#define	__BLOCKDEV_H__

enum BlockdevStatus {
	BLOCKDEV_STATUS_NOSIZE = -6,
	BLOCKDEV_STATUS_NOAVAIL = -5,
	BLOCKDEV_STATUS_ARGLEN = -4,
	BLOCKDEV_STATUS_READONLY = -3,
	BLOCKDEV_STATUS_RANGE = -2,
	BLOCKDEV_STATUS_BAD_DEV = -1,
	BLOCKDEV_STATUS_OK	= 0,
};


struct BlockdevHandler {
	int	(*create_from_arg)(const char *arg);
	int	(*read)(uint32_t device, uint32_t block, uint32_t count, uint32_t *data);
	int	(*write)(uint32_t device, uint32_t block, uint32_t count, uint32_t *data);
	int	(*blocksize)(uint32_t device);
};

#endif
