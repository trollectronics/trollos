#include "../process.h"
#include "../mmu.h"

static pid_t _fork(void) {
	Process *current, *new;
	
	current = process_from_pid(process_current());
	if(!(new = process_from_pid(process_create(current->user, current->group))))
		return -1;
	
	mmu_clone_userspace(&current->page_table, &new->page_table);
}

int32_t syscall_fork(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	return (int32_t) _fork();
}
