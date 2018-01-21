#include <trollos/process.h>

int32_t syscall_exit(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	process_exit(process_current(), arg0);
	return 0;
}
