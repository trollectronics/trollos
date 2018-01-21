#include <errno.h>
#include <trollos/process.h>
#include <trollos/vfs.h>

static inline ssize_t _write(int fd, const char *buf, size_t count) {
	Process *proc;
	int global_fd;
	
	if(fd < 0 || fd >= MAX_PROCESS_FILES)
		return -EBADF;
	
	if(!(proc = process_from_pid(process_current())))
		return -EINVAL;
	global_fd = proc->file[fd];
	
	if(global_fd < 0)
		return -EBADF;
	
	return vfs_write(global_fd, buf, count);
}

int32_t syscall_write(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return _write(arg0, (void *) arg1, arg2);
}
