#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <stdint.h>
#include <syscall.h>

#define _SYSCALL_HANDLER(x) int32_t x(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)

int32_t (*syscall_handler[SYSCALLS])(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int32_t syscall_stub(uint32_t syscall, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

_SYSCALL_HANDLER(syscall_exit);
_SYSCALL_HANDLER(syscall_fork);
_SYSCALL_HANDLER(syscall_read);
_SYSCALL_HANDLER(syscall_write);
_SYSCALL_HANDLER(syscall_open);
_SYSCALL_HANDLER(syscall_close);
_SYSCALL_HANDLER(syscall_waitpid);
_SYSCALL_HANDLER(syscall_creat);
_SYSCALL_HANDLER(syscall_link);
_SYSCALL_HANDLER(syscall_unlink);
_SYSCALL_HANDLER(syscall_execve);
_SYSCALL_HANDLER(syscall_chdir);
_SYSCALL_HANDLER(syscall_time);
_SYSCALL_HANDLER(syscall_mknod);
_SYSCALL_HANDLER(syscall_chmod);
_SYSCALL_HANDLER(syscall_lchown);
_SYSCALL_HANDLER(syscall_stat);
_SYSCALL_HANDLER(syscall_lseek);
_SYSCALL_HANDLER(syscall_getpid);

#endif
