#include <trollos/vfs.h>


int32_t syscall_write(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return vfs_write(arg0, (void *) arg1, arg2);
}
