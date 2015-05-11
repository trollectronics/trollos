#ifndef __INCLUDE_SYSCALL_H__
#define __INCLUDE_SYSCALL_H__
#include <stdint.h>

#define SYSCALL_TRAP 10

typedef enum Syscall Syscall;
enum Syscall {
	SYSCALL_EXIT,
	SYSCALL_FORK,
	SYSCALL_READ,
	SYSCALL_WRITE,
	SYSCALL_OPEN,
	SYSCALL_CLOSE,
	SYSCALL_WAITPID,
	SYSCALL_CREAT,
	SYSCALL_LINK,
	SYSCALL_UNLINK,
	SYSCALL_EXECVE,
	SYSCALL_CHDIR,
};

void *_syscall(uint32_t num, void *, void *, void *, void *, void *);

#endif
