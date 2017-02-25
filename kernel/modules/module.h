#ifndef __MODULE_H__
#define	__MODULE_H__

#define	MAX_MODULES		10

#include <stdint.h>
#include <sys/types.h>

struct ModuleCall {
	char		*name;
	int (*init)();
	int (*open)(void *ptr, uint32_t flags);
	int (*open_module)(void *aux, uint32_t major, uint32_t minor, uint32_t flags);
	off_t (*seek)(int minor, off_t offset, uint32_t whence);
	int (*write)(int minor, void *ptr, uint32_t count);
	off_t (*read)(int minor, void *ptr, off_t count);
	int32_t (*blksize)(int minor);
	int64_t (*devsize)(int minor);
};

int module_load(struct ModuleCall mc);
int module_init();

int module_open(uint32_t major, void *ptr, uint32_t length);
int module_open_device(uint32_t mod_major, void *aux, uint32_t major, uint32_t minor, uint32_t flags);
off_t module_seek(uint32_t major, int pid, off_t offset, uint32_t whence);
int module_write(uint32_t major, int pid, void *buf, uint32_t count);
int module_read(uint32_t major, int pid, void *buf, uint32_t count);
int32_t module_blksize(uint32_t major, int pid);
int64_t module_devsize(uint32_t major, int pid);

#endif
