/*
Copyright (c) 2018 Steven Arnow <s@rdw.se>
'vfs.c' - This file is part of trollos

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

#include "modules/fs/romfs.h"
#include <device.h>
#include <errno.h>
#include <sys/stat.h>
#include <trollos/vfs.h>
#include "util/log.h"
#include "util/printf.h"

static struct VFSFileEntry vfs_file[MAX_GLOBAL_FILES];

//static dev_t device;


static int _alloc_file() {
	int i;

	for (i = 0; i < MAX_GLOBAL_FILES; i++) {
		if (vfs_file[i].is_used)
			continue;
		vfs_file[i].is_used = true;
		return i;
	}

	return -EMFILE;
}


static int _extract_component(const char *str, char *component) {
	int i;

	for (i = 0; i < NAME_MAX && str[i]; i++)
		if ((component[i] = str[i]) == '/')
			break;
	component[i] = 0;
	if (str[i])
		i++;
	return i;
}


static int _resolv_ino(const char *path, dev_t *dev, ino_t *ino, int link) {
	char c[NAME_MAX + 1];
	int pos, err;
	ino_t dir;
	struct stat s;

	if (link >= 32)
		return -EMLINK;
	if (!path[0])
		return -EINVAL;
	if (path[0] != '/')
		return -ENOENT;
	
	dir = 0;
	pos = _extract_component(&path[1], c);
	if (path[pos] == '/')
		pos++;
	for (link = 0; path[pos]; ) {
		if ((err = fs_romfs_stat(dir, c, &s)) < 0)
			return err;
		if (S_ISDIR(s.st_mode)) {
			if (!path[pos]) 
				goto done;
			pos += _extract_component(&path[pos], c);
			dir = s.st_ino;
			continue;
		} else {
			return -ENOTDIR;
		}
	}
	
	if ((err = fs_romfs_stat(dir, c, &s)) < 0)
		return err;

done:

	*dev = s.st_dev;
	*ino = s.st_ino;
	return 0;
}


int vfs_stat(const char *path, struct stat *s) {
	int err;
	dev_t dev;
	ino_t ino;

	if ((err = _resolv_ino(path, &dev, &ino, 0)) < 0)
		return err;
	if ((err = fs_romfs_stat(ino, "", s)) < 0)
		return err;
	return 0;
}


off_t vfs_seek(int fd, off_t offset, int whence) {
	if (whence == 0) {
		return (vfs_file[fd].pos = offset);
	} else if (whence == 1) {
		return (vfs_file[fd].pos += offset);
	} else if (whence == 2) {
		return -ENOSYS;
		// TODO: Implement
	} else
		return -EINVAL;
}


ssize_t vfs_write(int fd, void *buf, size_t count) {
	struct Device *dev;
	
	kprintf(LOG_LEVEL_DEBUG, "Write to file %i from 0x%X (%lu)\n", fd, buf, count);
	
	if (!vfs_file[fd].is_device)
		return -EPERM;
	dev = device_lookup(vfs_file[fd].data.dev.dev);
	kprintf(LOG_LEVEL_DEBUG, "Device %u (0x%X)\n", vfs_file[fd].data.dev.dev, dev);
	if (dev->type == DEVICE_TYPE_BLOCK) {
		if (dev->blockdev.write) {
			return dev->blockdev.write(buf, count);
		} else {
			return -ENOSYS;
		}
	} else if (dev->type == DEVICE_TYPE_CHAR) {
		if (dev->chardev.write) {
			kprintf(LOG_LEVEL_DEBUG, "is chardev\n");
			return dev->chardev.write(buf, count);
		} else {
			return -ENOSYS;
		}
	} else
		return -ENOSYS;
}


ssize_t vfs_read(int fd, void *buf, size_t count) {
	struct Device *dev;
	ssize_t err;

	if (fd < -1 || fd >= MAX_GLOBAL_FILES)
		return -EBADF;
	if (!vfs_file[fd].is_used)
		return -EBADF;
	if (vfs_file[fd].is_device) {
		dev = device_lookup(vfs_file[fd].data.dev.dev);
		if (!dev)
			return -ENODEV;
		if (dev->type == DEVICE_TYPE_CHAR) {
			if (!dev->chardev.read)
				return -ENOSYS;
			return dev->chardev.read(buf, count);
		} else {
			if (!dev->blockdev.read)
				return -ENOSYS;
			if ((err = dev->blockdev.read(vfs_file[fd].data.dev.dev, vfs_file[fd].pos, buf, count)) > 0)
				vfs_file[fd].pos += err;
			return err;
		}
	} else {
		if (vfs_file[fd].directory)
			return -EISDIR;
		if (!vfs_file[fd].read)
			return -EACCES;
		err = fs_romfs_read(vfs_file[fd].data.fs.inode, buf, count, vfs_file[fd].pos);
		if (err > 0)
			vfs_file[fd].pos += err;
		return err;

	}
}


int vfs_read_directory(int fd, struct dirent *de, int dirs) {
	int i, err;
	struct dirent tde;

	if (fd < 0 || fd >= MAX_GLOBAL_FILES)
		return -EBADF;
	if (vfs_file[fd].is_device)
		return -ENOTDIR;
	if (!vfs_file[fd].directory)
		return -ENOTDIR;

	for (i = 0; i < dirs; i++) {
		if ((err = fs_romfs_read_directory(vfs_file[fd].data.fs.inode, vfs_file[fd].pos, &tde)) < 0)
			return err;
		if (!err)
			return i;
		de[i] = tde;
	}

	return i;
}


int vfs_open(const char *path, int flags) {
	int err;
	struct stat s;
	
	if ((err = vfs_stat(path, &s)) < 0) {
		kprintf(LOG_LEVEL_ERROR, "stat error in vfs open %s", path);
		return err;
	}
	if ((err = _alloc_file()) < 0)
		return err;

	vfs_file[err].mode = s.st_mode;
	vfs_file[err].is_device = false;
	vfs_file[err].read = true;
	vfs_file[err].write = false;
	vfs_file[err].unsupported = false;
	vfs_file[err].directory = false;
	vfs_file[err].pos = 0;
	vfs_file[err].data.fs.inode = s.st_ino;

	if (S_ISBLK(s.st_mode) || S_ISCHR(s.st_mode)) {
		vfs_file[err].is_device = true;
		vfs_file[err].data.dev.dev = s.st_rdev;
		vfs_file[err].write = true;
	} else if (S_ISDIR(s.st_mode)) {
		vfs_file[err].directory = true;
	} else if (S_ISREG(s.st_mode)) {
	} else {
		vfs_file[err].unsupported = false;
	}

	vfs_file[err].ref = 1;
	
	return err;
}


int vfs_open_device(dev_t device, int flags) {
	int fd;
	
	if ((fd = _alloc_file()) < 0)
		return fd;

	vfs_file[fd].mode = 0;
	vfs_file[fd].read = true;
	vfs_file[fd].unsupported = false;
	vfs_file[fd].directory = false;
	vfs_file[fd].pos = 0;
	vfs_file[fd].data.fs.inode = -1;

	vfs_file[fd].is_device = true;
	vfs_file[fd].data.dev.dev = device;
	vfs_file[fd].write = true;

	vfs_file[fd].ref = 1;
	
	kprintf(LOG_LEVEL_DEBUG, "Opened file %i as device %lu\n", fd, device);
	
	return fd;
}


int vfs_close(int fd) {
	if (fd < 0 || fd >= MAX_GLOBAL_FILES)
		return -EBADF;
	vfs_file[fd].ref--;
	if (!vfs_file[fd].ref)
		vfs_file[fd].is_used = false;
	return 0;
}
