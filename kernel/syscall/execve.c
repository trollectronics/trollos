#include <errno.h>
#include <stdint.h>
#include <trollos/process.h>
#include <fcntl.h>
#include <trollos/vfs.h>
#include "../util/mem.h"
#include "../modules/binformat/elf.h"
#include "../mmu.h"
#include "../util/log.h"

static inline int _execve(const char *filename, char *const argv[], char *const env[]) {
	Process *proc;

	if(!filename)
		return -EINVAL;
	
	pid_t pid = process_current();
	if(!(proc = process_from_pid(pid)))
		return -EINVAL;
	
	int fd;
	fd = vfs_open(filename, 0);
	vfs_seek(fd, 0, SEEK_END);
	size_t exe_size = vfs_tell(fd);
	vfs_seek(fd, 0, SEEK_SET);
	void *exe = kmalloc(exe_size);
	vfs_read(fd, exe, exe_size);
	vfs_close(fd);

	mmu_clear_userspace(&proc->userspace);
	//TODO: interpreter
	
	void *entry = elf_load(exe);
	process_set_pc(pid, entry);
	kprintf(LOG_LEVEL_INFO, "execve elf entry 0x%x\n", (uint32_t) entry);

	//TODO: set stack pointer?
	
	//TODO: push env
	//TODO: pusha args
	//
	
	syscall_trigger_context_switch();
	
	return 0;
}

int32_t syscall_execve(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	const char *filename = (const char *) arg0;
	char **const argv = (void *) arg1;
	char **const env = (void *) arg2;
	

	return _execve(filename, argv, env);
}
