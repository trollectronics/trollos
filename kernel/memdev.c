#include "memdev.h"
#include "string.h"
#include "blockdev.h"
#include <mem_addr.h>

struct Memdev memdev_state;
#define	KERNEL_ARGLEN_MAX	512


int memdev_from_arg(const char *opt) {
	char buff[KERNEL_ARGLEN_MAX], *next, *tok, *val;
	uint32_t devno, i;

	if (strnlen(opt, KERNEL_ARGLEN_MAX) >= KERNEL_ARGLEN_MAX)
		return BLOCKDEV_STATUS_ARGLEN;
	for (i = 0; i < MEMDEV_MAX; i++)
		if (!memdev_state.entry[i].size)
			break;
	if (i == MEMDEV_MAX)
		return BLOCKDEV_STATUS_NOAVAIL;
	devno = i;
	strncpy(buff, opt, 512);
	for (tok = strtok_r(buff, ",", &next); tok; tok = strtok_r(NULL, ",", &next)) {
		if ((val = strchr(tok, '='))) {
			if (!strcmp(tok, "addr"))
				memdev_state.entry[devno].ptr = (void *) str_parse_int(val);
			else if (!strcmp(tok, "size")) {
				if (!(memdev_state.entry[devno].size = str_parse_int(val)))
					return BLOCKDEV_STATUS_NOSIZE;
			} else if (!strcmp(tok, "blksize")) {
				if (!(memdev_state.entry[devno].block_size = str_parse_int(val)))
					memdev_state.entry[devno].block_size = 512;
			} else
				continue;
		}
	}
	
	printf("MEM BlockDev #%i @0x%X,size=0x%X\n", devno, *((uint32_t *) memdev_state.entry[devno].ptr), memdev_state.entry[devno].size);
	return devno;
}


int memdev_read(uint32_t device, uint32_t block, uint32_t count, uint32_t *dest) {
	uint32_t i, j;

	if (device >= MEMDEV_MAX)
		return BLOCKDEV_STATUS_BAD_DEV;
	if (!memdev_state.entry[device].size)
		return BLOCKDEV_STATUS_BAD_DEV;
	i = block * memdev_state.entry[device].block_size / 4;
	count = count * memdev_state.entry[device].block_size / 4 + i;
	if (count >= memdev_state.entry[device].size)
		return BLOCKDEV_STATUS_RANGE;
	for (j = 0; i < count; i++, j++)
		dest[j] = memdev_state.entry[device].ptr[i];
	return BLOCKDEV_STATUS_OK;
}


int memdev_write(uint32_t device, uint32_t block, uint32_t count, uint32_t *data) {
	return BLOCKDEV_STATUS_READONLY;
}
