#ifndef __MODULE_H__
#define	__MODULE_H__

#define	MAX_MODULES		10

#include <stdint.h>
#include <sys/types.h>

struct ModuleCall {
	char		*name;
	int (*init)();
	int (*open)(int pid, void *ptr, uint32_t flags);
	int (*open_module)(int pid, void *aux, uint32_t major, uint32_t minor, uint32_t flags);
	off_t (*seek)(int pid, int id, off_t offset, uint32_t whence);
	int (*write)(int pid, int id, void *ptr, uint32_t count);
	int (*read)(int pid, int id, void *ptr, uint32_t count);
	int32_t (*blksize)(int pid, int id);
};

int module_load(struct ModuleCall mc);
int module_init();

int module_open(uint32_t major, int pid, void *ptr, uint32_t length);
int module_open_device(uint32_t mod_major, int pid, void *aux, uint32_t major, uint32_t minor, uint32_t flags);
off_t module_seek(uint32_t major, int pid, int fd, off_t offset, uint32_t whence);
int module_write(uint32_t major, int pid, int fd, void *buf, uint32_t count);
int module_read(uint32_t major, int pid, int fd, void *buf, uint32_t count);
int module_blksize(uint32_t major, int pid, int fd);

#endif
