#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../../util/log.h"
#include "../../util/mem.h"
#include "../../util/string.h"
#include "blkcache.h"
#include "../../file.h"

#define	_ROMFS
#include "romfs.h"

static struct RomfsDescriptor romfs[MAX_ROMFS];
/* TODO: Rewrite to be endianess independent */


static void romfs_return(int d) {
	if (d < 0 || d >= MAX_ROMFS)
		return;
	romfs[d].blkcache.major = -1;
}


static int romfs_alloc() {
	int i;

	for (i = 0; i < MAX_ROMFS; i++)
		if (romfs[i].blkcache.major < 0)
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


int romfs_open(void *ptr, uint32_t flags) {
	return -EPERM;
}


int romfs_open_device(void *aux, uint32_t major, uint32_t minor, uint32_t flags, uint64_t *root_inode) {
	int fs;
	uint32_t i;
	char buff[256];

	if ((fs = romfs_alloc()) < 0)
		return fs;
	romfs[fs].blkcache = romfs_blkcache_open(major, minor);
	

	romfs_blkcache_seek(&romfs[fs].blkcache, 0, SEEK_SET);
	romfs_blkcache_read(&romfs[fs].blkcache, buff, 256);

	if (strncmp(buff, "-rom1fs-", 8))
		goto fail;

	/* TODO: Handle longer volume lables */
	for (i = 0; i < 256-16; i++)
		if (!buff[16 + i])
			goto end_found;

	return -EINVAL;
fail:
	romfs_return(fs);
	return -EINVAL;

end_found:
	if (i & 0xF)
		i += 0x10;
	i &= (~0xF);
	romfs[fs].inode_offset = (i >> 4);
	*root_inode = 0;

	return fs;
}


int romfs_inode_stat(int context, int64_t inode, const char *name, struct stat *st_s) {
	int i, t, link = 0;
	uint8_t buff[256];
	struct RomfsFileEntry *fe;
	struct stat st;

	inode += romfs[context].inode_offset;
	inode <<= 4;
	
	restart:
	if (link >= 16)
		return -EMLINK;

	if ((t = romfs_blkcache_seek(&romfs[context].blkcache, inode, 0)) < 0)
		return t;
	if ((i = romfs_blkcache_read(&romfs[context].blkcache, buff, 256)) < 0)
		return i;
	fe = (void *) buff;
	if ((buff[3] & 0x7) == 0) { // hard link
		link++;
		inode = fe->next_fileheader & (~0xF);
		goto restart;
	} else if ((buff[3] & 0x7) != 1) // directory
		return -ENOTDIR;
	
	for (inode = fe->next_fileheader; inode; inode = (fe->next_fileheader & (~0xF))) {
		if ((t = romfs_blkcache_seek(&romfs[context].blkcache, inode, 0)) < 0)
			return t;
		if ((i = romfs_blkcache_read(&romfs[context].blkcache, buff, 256)) < 0)
			return i;
		
		if (!(strncmp((char *) buff + 16, (char *) name, 256-16)))
			continue;
		goto found_it;
	}

	return -ENOENT;
found_it:
	// TODO: Replace this with a proper macro
	st.st_dev = makedev(romfs[context].blkcache.major, romfs[context].blkcache.minor);
	st.st_ino = (inode >> 4) - romfs[context].inode_offset;
	if ((fe->next_fileheader & 0x7) == 4 || (fe->next_fileheader & 0x7) == 5) {
		st.st_mode = 0500;
		st.st_rdev = fe->special_info;
	} else {
		st.st_mode = (fe->next_fileheader & 0x7) ? 0555 : 0444;
		st.st_rdev = 0;
	}

	st.st_nlink = 1;
	st.st_uid = st.st_gid = 0;
	st.st_size = fe->size;
	st.st_blksize = module_blksize(romfs[context].blkcache.major, romfs[context].blkcache.minor);
	st.st_blocks = fe->size / 512 + !!(fe->size & 0x1FF);
	st.st_mtime = 0;

	*st_s = st;
	return 0;
}

int romfs_read(int fs, int64_t inode, off_t offset, void *buf, uint32_t count) {
	int i, t;
	uint32_t seek, c;
	uint8_t buff[512];
	struct RomfsFileEntry *fe;

	if (fs < 0 || fs >= MAX_ROMFS)
		return -EINVAL;
	
	seek = ((romfs[fs].inode_offset + inode) << 4);
	
	romfs_blkcache_seek(&romfs[fs].blkcache, seek, 0);
	romfs_blkcache_read(&romfs[fs].blkcache, buff, 512);
	fe = (void *) buff;
	
	if (offset >= fe->size)
		return 0;
	if (offset + count >= fe->size)
		count = fe->size - offset;
	
	for (i = 16; i < 256; i++)
		if (!buff[i])
			goto end_of_fname;
	return -EIO;
end_of_fname:
	offset += (i & (~0xF)) + ((i & 0xF) ? 0x10 : 0);
	offset += inode;

	for (c = 0; c < count; c += i) {
		/* TODO: Buffer this better */
		seek = offset + c;
		if ((t = romfs_blkcache_seek(&romfs[fs].blkcache, seek, 0)) < 0)
			return t;
		if ((i = romfs_blkcache_read(&romfs[fs].blkcache, buff, 512)) <= 0)
			return i;
		memcpy(buf + c, buff, i);
	}

	return c;
}
