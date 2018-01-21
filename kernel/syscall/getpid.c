#include <trollos/process.h>

int32_t syscall_getpid(uint32_t syscall, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return (int32_t) process_current();
}
