#include <errno.h>
#include <stdbool.h>
#include "module.h"
#include "init_modules.h"


struct ModuleCall	mcarr[MAX_MODULES];


static int module_alloc() {
	int i;

	for (i = 0; i < MAX_MODULES; i++)
		if (!mcarr[i].init)
			return i;
	return -ENFILE;
}


static void module_release(int mod, bool deinit) {
	if (mod < 0 || mod >= MAX_MODULES)
		return;
	/* TODO: Call de-init */
	mcarr[mod].name = 0;
}


int module_load(struct ModuleCall mc) {
	int mod, ret;

	if ((mod = module_alloc()) < 0)
		return mod;
	mcarr[mod] = mc;
	if ((ret = mcarr[mod].init()) < 0)
		module_release(mod, false);

	return ret;
}


int module_init() {
	int i;

	for (i = 0; i < MAX_MODULES; i++)
		module_release(i, false);

	for (i = 0; module_init_list[i].name; i++)
		if (module_init_list[i].init)
			module_init_list[i].init();
	return 0;
}


int module_open(uint32_t major, int pid, void *ptr, uint32_t length) {
	if (major >= MAX_MODULES)
		return -EINVAL;
	if (!mcarr[major].open)
		return -EPERM;
	return mcarr[major].open(pid, ptr, length);
}


int module_open_device(uint32_t mod_major, int pid, void *aux, uint32_t major, uint32_t minor, uint32_t flags) {
	if (mod_major >= MAX_MODULES)
		return -EINVAL;
	if (!mcarr[major].open_module)
		return -EPERM;
	return mcarr[major].open_module(pid, aux, major, minor, flags);
}


off_t module_seek(uint32_t major, int pid, int fd, off_t offset, uint32_t whence) {
	if (major >= MAX_MODULES)
		return -EINVAL;
	if (!mcarr[major].seek)
		return -EPERM;
	return mcarr[major].seek(pid, fd, offset, whence);
}


int module_write(uint32_t major, int pid, int fd, void *buf, uint32_t count) {
	if (major >= MAX_MODULES)
		return -EINVAL;
	if (!mcarr[major].write)
		return -EPERM;
	return mcarr[major].write(pid, fd, buf, count);
}


int module_read(uint32_t major, int pid, int fd, void *buf, uint32_t count) {
	if (major >= MAX_MODULES)
		return -EINVAL;
	if (!mcarr[major].read)
		return -EPERM;
	return mcarr[major].read(pid, fd, buf, count);
}


int module_blksize(uint32_t major, int pid, int fd) {
	if (major >= MAX_MODULES)
		return -EINVAL;
	if (!mcarr[major].blksize)
		return -EPERM;
	return mcarr[major].blksize(pid, fd);
}
