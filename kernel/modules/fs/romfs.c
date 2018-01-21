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

#include "../../util/string.h"

#define	MAKE_INT(ptr)	((((uint8_t *) ptr)[0] << 24) | (((uint8_t *) ptr)[1] << 16) | (((uint8_t *) ptr)[2] << 8) | (((uint8_t *) ptr)[3]))

static dev_t device;
static ino_t root_inode;


static int _read_unaligned(dev_t dev, void *buf, ssize_t bytes, off_t offset) {
	ssize_t blksize, dobytes;
	uint8_t buff[blksize = device_lookup(dev)->blockdev.blksize(dev)];
	struct Device *d;

	if (offset < 0)
		return -EINVAL;
	if (!bytes)
		return 0;
	d = device_lookup(dev);
	for (; bytes;) {
		if (bytes <= (dobytes = (blksize - (offset % blksize))))
			dobytes = bytes;
		d->blockdev.read(dev, offset / blksize, buff, 1);
		memcpy(buf, buff + offset % blksize, dobytes);

		buf += dobytes;
		offset += dobytes;
		bytes -= dobytes;
	}

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


int fs_romfs_read_directory(ino_t inode, off_t pos, struct dirent *de) {
	uint8_t buff[16 + NAME_MAX + 1];
	int i;
	dev_t dev;
	
	inode += root_inode;

	if (pos < 0)
		return -EINVAL;
	_read_unaligned(device, buff, 16, inode * 16);
	
	if (inode != root_inode) {
		inode = MAKE_INT(buff + 4) >> 4;
		if ((buff[3] & 0x7) != 0x1)
			return -ENOTDIR;
	}
		
	for (i = 0; inode; i++) {
		_read_unaligned(device, buff, 16 + NAME_MAX + 1, inode * 16);
		if (i == pos)
			break;

		inode = MAKE_INT(buff) >> 4;
		continue;
	}

	if (!inode)
		return 0;

	de->d_ino = inode - root_inode;
	de->d_mode = _byte_to_mode(buff[3], MAKE_INT(buff + 4), &dev);
	strncpy(de->d_name, (char *) buff + 16, NAME_MAX);
	de->d_name[NAME_MAX] = 0;
	
	return sizeof(*de);
}


ssize_t fs_romfs_read(ino_t inode, void *data, size_t bytes, off_t offset) {
	uint8_t node[16];
	int i, j;
	
	inode += root_inode;
	_read_unaligned(device, node, 16, inode * 16);
	if (offset + bytes > MAKE_INT(node + 8))
		bytes = MAKE_INT(node + 8) - offset;
	for (i = 0; i < 16; i++) {
		_read_unaligned(device, node, 16, inode * 16 + 16 + i * 16);
		for (j = 0; j < 16; j++)
			if (!node[j])
				goto done;
	}
done:
	i++;
	_read_unaligned(device, data, bytes, inode * 16 + 16 + i * 16 + offset);
	return bytes;
}


int fs_romfs_stat(ino_t inode, const char *fname, struct stat *s) {
	uint8_t buff[16+NAME_MAX+1];
	struct Device *d;

	d = device_lookup(device);
	inode += root_inode;
	_read_unaligned(device, buff, 16+NAME_MAX+1, inode * 16);
	if (!*fname)
		goto stat;
	if (inode != root_inode) {
		if ((buff[3] & 0x7) != 1)
			return -ENOTDIR;
		inode = MAKE_INT(buff + 4) >> 4;
	}
	for (; inode; inode = MAKE_INT(buff) >> 4) {
		_read_unaligned(device, buff, 16+NAME_MAX+1, inode * 16);
		if (!strncmp(fname, (char *) buff + 16, NAME_MAX))
			goto stat;
	}

	return -ENOENT;
stat:
	s->st_dev = device;
	s->st_ino = inode - root_inode;
	s->st_rdev = 0;
	s->st_mode = _byte_to_mode(buff[3], MAKE_INT(buff + 4), &s->st_rdev);
	s->st_nlink = 1;
	s->st_uid = 0;
	s->st_gid = 0;
	s->st_size = MAKE_INT(buff + 8);
	s->st_blksize = d->blockdev.blksize(device);
	s->st_blocks = (MAKE_INT(buff + 8) + d->blockdev.blksize(device) - 1) / d->blockdev.blksize(device);
	s->st_mtime = 0;

	return 0;
}


int fs_romfs_instantiate(dev_t dev) {
	uint8_t buff[512];
	int i;

	device = dev;

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
	root_inode = i / 16;

	return 0;
}
