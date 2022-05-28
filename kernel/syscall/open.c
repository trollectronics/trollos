#include <errno.h>
#include <trollos/process.h>
#include <trollos/vfs.h>

#include "../util/log.h"

static inline int _open(const char *filename, int flags) {
	Process *proc;
	int global_fd;
	int fd;

	kprintf(LOG_LEVEL_INFO, "open %s (%x): %i\n", filename, (uint32_t) filename, flags);

	if(!filename)
		return -EINVAL;
	
	if(!(proc = process_from_pid(process_current())))
		return -EINVAL;
	
	if((global_fd = vfs_open(filename, flags)) < 0)
		return global_fd;
	
	for(fd = 0; fd < MAX_PROCESS_FILES; fd++)
		if(proc->file[fd] < 0) {
			proc->file[fd] = global_fd;
			
			return fd;
		}
	
	
	
	return -EMFILE;
}

int32_t syscall_open(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return _open((void *) arg0, arg1);
}
