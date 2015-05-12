#include <stdint.h>
#include <errno.h>
#include "../../util/log.h"
#include "../module.h"

#define	_ROMFS
#include "romfs.h"

static struct RomfsDescriptor romfs[MAX_ROMFS];


static void romfs_return(int d) {
	if (d < 0 || d >= MAX_ROMFS)
		return;
	romfs[d].blockdev.fd = -1;
}


static int romfs_alloc() {
	int i;

	for (i = 0; i < MAX_ROMFS; i++)
		if (romfs[i].blockdev.fd < 0)
			return i;
	return -ENOENT;
}


int romfs_init() {
	int i;

	for (i = 0; i < MAX_ROMFS; i++)
		romfs_return(i);
	kprintf(LOG_LEVEL_INFO, "[romfs] Module initialized\n");
	return 0;
}


int romfs_open(int pid, void *ptr, uint32_t flags) {
	return -EPERM;
}


int romfs_open_device(int pid, void *aux, uint32_t major, uint32_t minor, uint32_t flags) {
	int fs;
	char buff[8];

	if ((fs = romfs_alloc()) < 0)
		return fs;
	romfs[fs].blockdev.major = major;
	romfs[fs].blockdev.minor = minor;
	
	module_seek(major, pid, minor, 0);
	module_read(major, pid, minor, buff, 8);

	//TODO: memcmp
	if (strncmp(buff, "-rom1fs-", 8)) {
		romfs_return(fs);
		return -EINVAL;
	}

	return fs;
}
