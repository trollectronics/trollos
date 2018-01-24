#include <errno.h>
#include "util/log.h"
#include "kernel.h"
#include "syscall.h"

int32_t (*syscall_handler[SYSCALLS])(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) = {
	[SYSCALL_EXIT] = syscall_exit,
	[SYSCALL_FORK] = syscall_fork,
	[SYSCALL_READ] = syscall_read,
	[SYSCALL_WRITE] = syscall_write,
	[SYSCALL_OPEN] = syscall_open,
	[SYSCALL_CLOSE] = syscall_close,
	[SYSCALL_WAITPID] = syscall_waitpid,
	[SYSCALL_CREAT] = syscall_creat,
	[SYSCALL_LINK] = syscall_link,
	[SYSCALL_UNLINK] = syscall_unlink,
	[SYSCALL_EXECVE] = syscall_execve,
	[SYSCALL_CHDIR] = syscall_chdir,
	[SYSCALL_TIME] = syscall_time,
	[SYSCALL_MKNOD] = syscall_mknod,
	[SYSCALL_CHMOD] = syscall_chmod,
	[SYSCALL_LCHOWN] = syscall_lchown,
	[SYSCALL_STAT] = syscall_stat,
	[SYSCALL_LSEEK] = syscall_lseek,
	[SYSCALL_GETPID] = syscall_getpid,
};

int32_t syscall_stub(uint32_t syscall, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	int32_t (*call)(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);
	
	if(syscall > SYSCALLS) {
		kprintf(LOG_LEVEL_WARNING, "SYSCALL: invalid syscall %i called with %u %u %u %u %u\n", syscall, arg0, arg1, arg2, arg3, arg4);
		return -EINVAL;
	}
	kprintf(LOG_LEVEL_DEBUG, "SYSCALL: %i called\n", syscall);
	call = syscall_handler[syscall];
	return call(arg0, arg1, arg2, arg3, arg4);
}


int32_t syscall_close(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_waitpid(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_creat(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_link(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_unlink(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_chdir(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_time(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_mknod(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_chmod(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_lchown(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_stat(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}

int32_t syscall_lseek(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return 0;
}
