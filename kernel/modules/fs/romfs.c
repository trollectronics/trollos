/*
Copyright (c) 2018 Steven Arnow <s@rdw.se>
'romfs.c' - This file is part of trollos

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#include <device.h>
#include <dirent.h>
#include "limits.h"

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <trollos/vfs.h>

#include "../../util/log.h"

#include "../../util/string.h"

#define	MAX_ROMFS 2

#define	MAKE_INT(ptr)	((((uint8_t *) ptr)[0] << 24) | (((uint8_t *) ptr)[1] << 16) | (((uint8_t *) ptr)[2] << 8) | (((uint8_t *) ptr)[3]))

struct Instance {
	int used;
	dev_t device;
	ino_t root_inode;
};


static struct Instance _instance[MAX_ROMFS];

static int _read_unaligned(dev_t dev, void *buf, ssize_t bytes, off_t offset) {
	/*ssize_t blksize, dobytes;
	uint8_t buff[blksize = device_lookup(dev)->blockdev.blksize(dev)];*/
	struct Device *d;

	if (offset < 0)
		return -EINVAL;
	if (!bytes)
		return 0;
	d = device_lookup(dev);

	/* As it turns out, this is not how block devices work in TrollOS */
	/*for (; bytes;) {
		if (bytes <= (dobytes = (blksize - (offset % blksize))))
			dobytes = bytes;
		d->blockdev.read(dev, offset, buff, 1);
		memcpy(buf, buff + offset % blksize, dobytes);

		buf += dobytes;
		offset += dobytes;
		bytes -= dobytes;
	}*/
	
	
	d->blockdev.read(dev, offset, buf, bytes);

	return 0;
}


static mode_t _byte_to_mode(uint8_t byte, uint32_t special, dev_t *rdev) {
	mode_t mode;
	if ((byte & 0x7) == 0x1)
		mode = S_IFDIR | 0444;
	else if ((byte & 0x7) == 0x2)
		mode = S_IFREG | 0444;
	else if ((byte & 0x7) == 0x3)
		mode = S_IFLNK | 0444;
	else if ((byte & 0x7) == 0x4)
		mode = S_IFBLK, *rdev = makedev(special >> 16, special & 0xFFFF);
	else if ((byte & 0x7) == 0x5)
		mode = S_IFCHR, *rdev = makedev(special >> 16, special & 0xFFFF);
	else if ((byte & 0x7) == 0x6)
		mode = S_IFSOCK | 0444;
	else
		mode = S_IFIFO | 0444;
	if (byte & 0x8)
		mode |= 0111;
	return mode;
}


int fs_romfs_read_directory(int instance, ino_t inode, off_t pos, struct dirent *de) {
	uint8_t buff[16 + NAME_MAX + 1];
	int i;
	dev_t dev;
	
	inode += _instance[instance].root_inode;

	if (pos < 0)
		return -EINVAL;
	_read_unaligned(_instance[instance].device, buff, 16, inode * 16);
	
	if (inode != _instance[instance].root_inode) {
		inode = MAKE_INT(buff + 4) >> 4;
		if ((buff[3] & 0x7) != 0x1)
			return -ENOTDIR;
	}
		
	for (i = 0; inode; i++) {
		_read_unaligned(_instance[instance].device, buff, 16 + NAME_MAX + 1, inode * 16);
		if (i == pos)
			break;

		inode = MAKE_INT(buff) >> 4;
		continue;
	}

	if (!inode)
		return 0;

	de->d_ino = inode - _instance[instance].root_inode;
	de->d_mode = _byte_to_mode(buff[3], MAKE_INT(buff + 4), &dev);
	strncpy(de->d_name, (char *) buff + 16, NAME_MAX);
	de->d_name[NAME_MAX] = 0;
	
	return sizeof(*de);
}


ssize_t fs_romfs_read(int instance, ino_t inode, void *data, size_t bytes, off_t offset) {
	uint8_t node[16];
	int i, j;
	
	inode += _instance[instance].root_inode;
	_read_unaligned(_instance[instance].device, node, 16, inode * 16);
	if (offset + bytes > MAKE_INT(node + 8))
		bytes = MAKE_INT(node + 8) - offset;
	for (i = 0; i < 16; i++) {
		_read_unaligned(_instance[instance].device, node, 16, inode * 16 + 16 + i * 16);
		for (j = 0; j < 16; j++)
			if (!node[j])
				goto done;
	}
done:
	i++;
	_read_unaligned(_instance[instance].device, data, bytes, inode * 16 + 16 + i * 16 + offset);
	return bytes;
}


int fs_romfs_stat_inode(int instance, ino_t inode, struct stat *s) {
	uint8_t buff[16];
	struct Device *d;

	d = device_lookup(_instance[instance].device);
	
	inode += _instance[instance].root_inode;
	_read_unaligned(_instance[instance].device, buff, 16, inode * 16);
	
	s->st_dev = _instance[instance].device;
	s->st_ino = inode - _instance[instance].root_inode;
	s->st_rdev = 0;
	s->st_mode = _byte_to_mode(buff[3], MAKE_INT(buff + 4), &s->st_rdev);
	s->st_nlink = 1;
	s->st_uid = 0;
	s->st_gid = 0;
	s->st_size = MAKE_INT(buff + 8);
	s->st_blksize = d->blockdev.blksize(_instance[instance].device);
	s->st_blocks = (MAKE_INT(buff + 8) + d->blockdev.blksize(_instance[instance].device) - 1) / d->blockdev.blksize(_instance[instance].device);
	s->st_mtime = 0;

	return 0;
}


int fs_romfs_stat(int instance, ino_t inode, const char *fname, struct stat *s) {
	uint8_t buff[16+NAME_MAX+1];
	
	inode += _instance[instance].root_inode;
	_read_unaligned(_instance[instance].device, buff, 16+NAME_MAX+1, inode * 16);
	if (!*fname)
		goto stat;
	if (inode != _instance[instance].root_inode) {
		if ((buff[3] & 0x7) != 1)
			return -ENOTDIR;
		inode = MAKE_INT(buff + 4) >> 4;
	}
	for (; inode; inode = MAKE_INT(buff) >> 4) {
		_read_unaligned(_instance[instance].device, buff, 16+NAME_MAX+1, inode * 16);
		if (!strncmp(fname, (char *) buff + 16, NAME_MAX))
			goto stat;
	}

	return -ENOENT;
stat:
	return fs_romfs_stat_inode(instance, inode - _instance[instance].root_inode, s);
}


int fs_romfs_mount(dev_t dev, ino_t *root_inode) {
	uint8_t buff[512];
	int i, q;
	
	for (q = 0; q < MAX_ROMFS; q++)
		if (_instance[q].used)
			continue;
	if (q == MAX_ROMFS)
		return -ENOMEM;
	_instance[q].device = dev;

	_read_unaligned(dev, buff, 512, 0);
	
	if (strncmp("-rom1fs-", (char *) buff, 8))
		return -EINVAL;

	for (i = 16; i < 512; i++) {
		if (buff[i])
			continue;
		break;
	}

	if (i == 512)
		return -EINVAL;
	i += 16;
	_instance[q].root_inode = i / 16;
	*root_inode = 0;
	_instance[q].used = 1;

	return q;
}


int fs_romfs_init() {
	struct VFSFSType instance;

	strcpy(instance.name, "romfs");
	instance._stat = fs_romfs_stat;
	instance._stat_inode = fs_romfs_stat_inode;
	instance._read = fs_romfs_read;
	instance._read_directory = fs_romfs_read_directory;
	instance._mount = fs_romfs_mount;
	
	return vfs_fs_register(&instance);
}
