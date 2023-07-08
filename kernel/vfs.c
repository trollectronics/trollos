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
#include "util/string.h"

static struct VFSFileEntry vfs_file[MAX_GLOBAL_FILES];
static struct VFSFSType _vfs_type[MAX_FILE_SYSTEMS];

struct VFSMountPoint {
	int		is_used;
	int		open_files;
	dev_t		dev;
	ino_t		inode;

	int		mounted_fs;
	int		mounted_instance;
	dev_t		mounted_dev;
	ino_t		mounted_ino;
};


static struct VFSMountPoint _vfs_mount[MAX_MOUNT_POINTS];
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


static int _is_mountpoint(dev_t dev, ino_t ino) {
	int i;

	for (i = 0; i < MAX_MOUNT_POINTS; i++) {
		if (!_vfs_mount[i].is_used)
			continue;
		if ((_vfs_mount[i].dev == dev && _vfs_mount[i].inode == ino))
			return i;
	}

	return -1;
}


static int _resolv_ino(const char *path, int *mp_out, ino_t *ino, int link) {
	char c[NAME_MAX + 1];
	int pos, err, cur_fs, cur_fsi, i, mp;
	ino_t dir;
	struct stat s;

	if (link >= 32)
		return -EMLINK;
	if (!path[0])
		return -EINVAL;
	if (path[0] != '/')
		return -ENOENT;

	for (i = 0; i < MAX_MOUNT_POINTS; i++) {
		if (!_vfs_mount[i].is_used)
			continue;
		if (_vfs_mount[i].dev != ~0 || _vfs_mount[i].inode != ~0)
			continue;
		break;
	}

	if (i == MAX_MOUNT_POINTS)
		return -ENOENT;
	
	
	cur_fs = _vfs_mount[i].mounted_fs;
	cur_fsi = i;
	
	dir = 0;
	pos = _extract_component(&path[1], c) + 1;
	while (path[pos] == '/')
		pos++;
	for (link = 0; path[pos]; ) {
		if ((err = _vfs_type[cur_fs]._stat(_vfs_mount[cur_fsi].mounted_instance, dir, c, &s)) < 0) {
			return err;
		}
		if ((mp = _is_mountpoint(s.st_dev, s.st_ino)) >= 0) {
			cur_fs = _vfs_mount[mp].mounted_fs;
			cur_fsi = mp;
			dir = _vfs_mount[mp].mounted_ino;
			pos += _extract_component(&path[pos], c);	
			continue;
		}

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

	if ((err = _vfs_type[cur_fs]._stat(_vfs_mount[cur_fsi].mounted_instance, dir, c, &s)) < 0) {
		return err;
	}

done:
	*mp_out = cur_fsi;
	*ino = s.st_ino;
	return 0;
}


int vfs_stat(const char *path, int *mp_out, struct stat *s) {
	int err, mp;
	ino_t ino;

	if ((err = _resolv_ino(path, &mp, &ino, 0)) < 0) {
		kprintf(LOG_LEVEL_ERROR, "vfs_stat failed to resolve inode for %s (%i)\n", path, err);
		return err;
	}
	if ((err = _vfs_type[_vfs_mount[mp].mounted_fs]._stat_inode(_vfs_mount[mp].mounted_instance, ino, s)) < 0)
		return err;
	if (mp_out)
		*mp_out = mp;
	return 0;
}


static int vfs_stat_inode(int instance, ino_t ino, struct stat *s) { // Must *not* be exposed to userspsace
	return _vfs_type[_vfs_mount[instance].mounted_fs]._stat_inode(_vfs_mount[instance].mounted_instance, ino, s);
}


off_t vfs_tell(int fd) {
	return vfs_file[fd].pos;
}


off_t vfs_seek(int fd, off_t offset, int whence) {
	if (whence == 0) {
		return vfs_file[fd].pos = offset, 0;
	} else if (whence == 1) {
		return vfs_file[fd].pos += offset, 0;
	} else if (whence == 2) {
		struct stat s;
		vfs_stat_inode(vfs_file[fd].data.fs.instance, vfs_file[fd].data.fs.inode, &s);
		vfs_file[fd].pos = s.st_size + offset;
		return 0;
	} else
		return -EINVAL;
}


ssize_t vfs_write(int fd, void *buf, size_t count) {
	struct Device *dev;
	
	kprintf(LOG_LEVEL_SPAM, "Write to file %i from 0x%X (%lu)\n", fd, buf, count);
	
	if (!vfs_file[fd].is_device)
		return -EPERM;
	dev = device_lookup(vfs_file[fd].data.dev.dev);
	kprintf(LOG_LEVEL_SPAM, "Device %u (0x%X)\n", vfs_file[fd].data.dev.dev, dev);
	if (dev->type == DEVICE_TYPE_BLOCK) {
		if (dev->blockdev.write) {
			return dev->blockdev.write(buf, count);
		} else {
			return -ENOSYS;
		}
	} else if (dev->type == DEVICE_TYPE_CHAR) {
		return chardev_write(dev, buf, count);
	} else
		return -ENOSYS;
}


ssize_t vfs_read(int fd, void *buf, size_t count) {
	struct Device *dev;
	ssize_t err;
	int mp;

	if (fd < -1 || fd >= MAX_GLOBAL_FILES)
		return -EBADF;
	if (!vfs_file[fd].is_used)
		return -EBADF;

	mp = vfs_file[fd].data.fs.instance;

	if (vfs_file[fd].is_device) {
		dev = device_lookup(vfs_file[fd].data.dev.dev);
		if (!dev)
			return -ENODEV;
		if (dev->type == DEVICE_TYPE_CHAR) {
			return chardev_read(dev, buf, count);
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
		err = _vfs_type[_vfs_mount[mp].mounted_fs]._read(_vfs_mount[mp].mounted_instance, vfs_file[fd].data.fs.inode, buf, count, vfs_file[fd].pos);
		if (err > 0)
			vfs_file[fd].pos += err;
		return err;

	}
}


int vfs_read_directory(int fd, struct dirent *de, int dirs) {
	int i, err, mp;
	struct dirent tde;

	if (fd < 0 || fd >= MAX_GLOBAL_FILES)
		return -EBADF;
	if (vfs_file[fd].is_device)
		return -ENOTDIR;
	if (!vfs_file[fd].directory)
		return -ENOTDIR;
	
	mp = vfs_file[fd].data.fs.instance;

	for (i = 0; i < dirs; i++) {
		if ((err = _vfs_type[_vfs_mount[mp].mounted_fs]._read_directory(_vfs_mount[mp].mounted_instance, vfs_file[fd].data.fs.inode, vfs_file[fd].pos, &tde)) < 0)
			return err;
		if (!err)
			return i;
		de[i] = tde;
	}

	return i;
}


int vfs_open(const char *path, int flags) {
	int err, mp;
	struct stat s;
	
	if ((err = vfs_stat(path, &mp, &s)) < 0) {
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
	vfs_file[err].data.fs.instance = mp;

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
	_vfs_mount[mp].open_files++;
	
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
	
	kprintf(LOG_LEVEL_SPAM, "Opened file %i as device %lu\n", fd, device);
	
	return fd;
}


int vfs_close(int fd) {
	if (fd < 0 || fd >= MAX_GLOBAL_FILES)
		return -EBADF;
	vfs_file[fd].ref--;
	if (!vfs_file[fd].ref) {
		vfs_file[fd].is_used = false;
		_vfs_mount[vfs_file[fd].data.fs.instance].open_files--;
	}

	return 0;
}


int vfs_fs_register(struct VFSFSType *fs) {
	int i, q;
	struct VFSFSType _fs = *fs;

	_fs.name[15] = 0;
	
	q = -1;

	for (i = 0; i < MAX_FILE_SYSTEMS; i++) {
		if (_vfs_type[i].used) {
			if (!strcmp(_vfs_type[i].name, _fs.name))
				return -EEXIST;
			continue;
		}

		q = i;
	}

	if (q < 0)
		return -ENOMEM;

	_vfs_type[q] = _fs;
	_vfs_type[q].used = 1;
	return q;
}


int vfs_mount(dev_t dev, const char *path, const char *fs) {
	int i, instance;
	struct stat s;

	for (i = 0; i < MAX_FILE_SYSTEMS; i++) {
		if (!_vfs_type[i].used)
			continue;
		if (strcmp(_vfs_type[i].name, fs))
			continue;
		break;
	}

	if (i == MAX_FILE_SYSTEMS)
		return -EINVAL;
	
	
	for (instance = 0; instance < MAX_MOUNT_POINTS; instance++)
		if (!_vfs_mount[instance].is_used)
			break;
	if (instance == MAX_MOUNT_POINTS)
		return -ENOMEM;
	
	
	if (path[0] == '/' && !path[1]) {
		_vfs_mount[instance].dev = ~0;
		_vfs_mount[instance].inode = ~0;
	} else {
		int err, parent;
		if ((err = vfs_stat(path, &parent, &s)) < 0)
			return err;
		_vfs_mount[instance].dev = s.st_dev;
		_vfs_mount[instance].inode = s.st_ino;
		_vfs_mount[parent].open_files++;
	}
	
	if ((_vfs_mount[instance].mounted_instance = _vfs_type[i]._mount(dev, &_vfs_mount[instance].mounted_ino)) < 0)
		return _vfs_mount[instance].mounted_instance;
	_vfs_mount[instance].mounted_dev = dev;
	_vfs_mount[instance].mounted_fs = i;
	_vfs_mount[instance].open_files = 0;
	_vfs_mount[instance].is_used = 1;

	return 0;
}
