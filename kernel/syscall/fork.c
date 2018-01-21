#include <stdint.h>
#include <errno.h>
#include "../util/mem.h"
#include "../util/log.h"
#include <trollos/process.h>
#include "../mmu.h"

static pid_t _fork(void) {
	Process *current, *new;
	pid_t pid;
	
	if(!(current = process_from_pid(process_current())))
		return -EAGAIN;
	if((pid = process_create(current->user, current->group)) < 0)
		return -EAGAIN;
	if(!(new = process_from_pid(pid)))
		return -EAGAIN;
	
	kprintf(LOG_LEVEL_DEBUG, "fork: created process %i\n", new->pid);
	if(mmu_clone_userspace(&current->userspace, &new->userspace) < 0) {
		process_exit(pid, 1);
		new->state = PROCESS_STATE_ZOMBIE;
		process_wait(pid);
		return -ENOMEM;
	}
	kprintf(LOG_LEVEL_DEBUG, "fork: cloned userspace\n");
	new->reg.pc = current->reg.pc;
	new->reg.stack = current->reg.stack;
	memcpy(new->reg.general, current->reg.general, sizeof(new->reg.general));
	memcpy(new->file, current->file, sizeof(int32_t)*MAX_PROCESS_FILES);
	new->reg.general[0] = 0; /* Fork returns 0 for child */
	new->parent = current->pid;
	
	return new->pid;
}

int32_t syscall_fork(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return (int32_t) _fork();
}
