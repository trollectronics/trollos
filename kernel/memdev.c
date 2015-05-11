#include "memdev.h"
#include "string.h"
#include "blockdev.h"
#include "printf.h"
#include <mem_addr.h>

struct Memdev memdev_state;

int memdev_read(uint32_t device, uint32_t block, uint32_t count, uint32_t *dest);
int memdev_write(uint32_t device, uint32_t block, uint32_t count, uint32_t *data);
int memdev_blocksize(uint32_t device);

bool memdev_init() {
	int i;
	struct BlockdevHandler bd;

	for (i = 0; i < MEMDEV_MAX; i++)
		memdev_state.entry[i].size = 0;
	
	bd.create_from_arg = memdev_from_arg;
	bd.read = memdev_read;
	bd.write = memdev_write;
	bd.unload = NULL;
	bd.blocksize = memdev_blocksize;
	strncpy(bd.name, "memdev", 32);
	if ((memdev_state.iface = blockdev_iface_add(bd)) < 0) {
		memdev_state.entry[i].size = 0;
		return false;
	}

	return true;
}


int memdev_new() {
	int i;

	for (i = 0; i < MEMDEV_MAX; i++)
		if (!memdev_state.entry[i].size)
			break;
	if (i == MEMDEV_MAX)
		return BLOCKDEV_STATUS_NOAVAIL;
	memdev_state.entry[i].ptr = 0;
	memdev_state.entry[i].size = 0;
	memdev_state.entry[i].block_size = 512;
	memdev_state.entry[i].writable = false;
	
	return i;
}

int memdev_from_arg(const char *opt) {
	char buff[STRING_ARG_MAX], *next, *tok, *val;
	uint32_t devno, i;

	if (strnlen(opt, STRING_ARG_MAX) >= STRING_ARG_MAX)
		return BLOCKDEV_STATUS_ARGLEN;
	devno = memdev_new();
	if (devno < 0)
		return BLOCKDEV_STATUS_NOAVAIL;
	strncpy(buff, opt, 512);
	for (tok = strtok_r(buff, ",", &next); tok; tok = strtok_r(NULL, ",", &next)) {
		if ((val = strchr(tok, '='))) {
			*val = 0, val++;
			if (!strcmp(tok, "addr")) {
				memdev_state.entry[devno].ptr = (void *) str_parse_int(val);
			} else if (!strcmp(tok, "size")) {
				if (!(memdev_state.entry[devno].size = str_parse_int(val)))
					return BLOCKDEV_STATUS_NOSIZE;
			} else if (!strcmp(tok, "blksize")) {
				if (!(memdev_state.entry[devno].block_size = str_parse_int(val)))
					memdev_state.entry[devno].block_size = 512;
			} else
				continue;
		}
	}
	
	if ((i = blockdev_add("memblk", memdev_state.iface, devno)) < 0) {

	memdev_state.iface = i;
	printf("MEM BlockDev #%i @0x%X,size=0x%X\n", devno, ((uint32_t *) memdev_state.entry[devno].ptr), memdev_state.entry[devno].size);
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


int memdev_blocksize(uint32_t device) {
	if (device >= MEMDEV_MAX)
		return -1;
	if (!memdev_state.entry[device].size)
		return -1;
	return memdev_state.entry[device].block_size;
}
