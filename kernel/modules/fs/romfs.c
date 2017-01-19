#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include "../../util/log.h"
#include "../../util/mem.h"
#include "../../util/string.h"
#include "../module.h"
#include "file.h"

#define	_ROMFS
#include "romfs.h"

static struct RomfsDescriptor romfs[MAX_ROMFS];
/* TODO: Rewrite to be endianess independent */


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


int romfs_open_device(int pid, void *aux, uint32_t major, uint32_t minor, uint32_t flags, uint64_t *root_inode) {
	int fs;
	uint32_t i;
	char buff[512];

	if ((fs = romfs_alloc()) < 0)
		return fs;
	romfs[fs].blockdev.major = major;
	romfs[fs].blockdev.minor = minor;
	
	module_seek(major, pid, minor, 0, SEEK_SET);
	module_read(major, pid, minor, buff, 512);

	//TODO: memcmp
	if (strncmp(buff, "-rom1fs-", 8)) {
		romfs_return(fs);
		return -EINVAL;
	}

	/* TODO: Handle longer volume lables */
	for (i = 0; i < 512-16; i++) {
		if (!buff[16 + i])
			goto end_found;
		break;
	}

	return -EINVAL;

end_found:
	if (i & 0xF)
		i++;
	i &= (~0xF);
	romfs[fs].inode_offset = (i >> 4);
	*root_inode = 0;

	return fs;
}


int64_t romfs_inode_lookup(int context, int64_t inode, const char *name) {
	int i, t;
	uint8_t buff[512];
	inode += romfs[context].inode_offset;
	inode <<= 4;
	
	if ((t = module_seek(romfs[context].blockdev.major, 0, romfs[context].blockdev.minor, inode & (~0x1FF), 0)) < 0)
		return t;
	if ((i = module_read(romfs[context].blockdev.major, 0, romfs[context].blockdev.minor, buff, 512)) < 0)
		return i;
	if ((buff[(inode & 0x1FF) + 3] & 0x7) != 1)
		return -ENOTDIR;
	
	return -ENOTDIR;
}


int romfs_read(int pid, int fs, void *buf, uint32_t count) {
	int i, t;
	uint32_t seek, c;
	uint8_t buff[512];
	struct RomfsFileEntry *fe;

	if (fs < 0 || fs >= MAX_ROMFS)
		return -EINVAL;
	if (romfs[fs].blockdev.fd < 0)
		return -EBADF;

	
	seek = (romfs[fs].cur_inode + romfs[fs].inode_offset) << 4;
	// We're in luck, romfs is always aligned on 16-byte boundary, and file entries are 16 bytes
	module_seek(romfs[fs].blockdev.major, pid, romfs[fs].blockdev.minor, seek & (~0x1FF), 0);
	module_read(romfs[fs].blockdev.major, pid, romfs[fs].blockdev.minor, buff, 512);
	fe = (void *) buff + (seek & 0x1F0);
	
	if (vfs_file_descriptor[romfs[fs].blockdev.fd].pos >= fe->size)
		return 0;
	if (vfs_file_descriptor[romfs[fs].blockdev.fd].pos + c >= fe->size)
		c = fe->size - vfs_file_descriptor[romfs[fs].blockdev.fd].pos;

	for (c = 0; c < count; c += i) {
		/* TODO: Buffer this better */
		seek = (16 + vfs_file_descriptor[romfs[fs].blockdev.fd].pos + c + (romfs[fs].cur_inode + romfs[fs].inode_offset) * 16);
		i = 512 - (seek & 0x1FF);
		if ((t = module_seek(romfs[fs].blockdev.major, pid, romfs[fs].blockdev.minor, seek & (~0x1FF), 0)) < 0)
			return t;
		if ((i = module_read(romfs[fs].blockdev.major, pid, romfs[fs].blockdev.minor, buff, 512)) < 0)
			return i;
		if (!c)
			memcpy_to_user(buf, buff + 512 - i, i);
		else
			memcpy_to_user(buf, buff, i);
	}

	return c;
}
