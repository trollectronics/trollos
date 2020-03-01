/*
Copyright (c) 2019 Steven Arnow <s@rdw.se>
'devfs.c' - This file is part of trollos

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
#include <limits.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>


int fs_devfs_stat(ino_t inode, const char *fnmae, struct stat *s) {
	// TODO: make devfs something with a device number
	dev_t dev;
	Device d;

	if (!inode)
		return -ENOTDIR;
	if (!(dev = device_lookup_name(fname, &d)))
		return -ENOENT;
	s->st_dev = 0;
	s->st_ino = dev;
	if (d.type == DEVICE_TYPE_CHAR)
		s->st_mode = S_IRUSR | S_IWUSR | S_IFCHR;
	else if (d.type == DEVICE_TYPE_BLOCK)
		s->st_mode = S_IRUSR | S_IWUSR | S_IFBLK;
	
	s->st_nlink = 1;
	s->st_uid = 0;
	s->st_gid = 0;
	s->st_rdev = dev;
	s->st_size = 0;
	s->st_blksize = 512;
	s->st_blocks = 0;
	s->st_mtime = 0;
}

int fs_devfs_instantiate(dev_t dev) {
	return 0;
}



