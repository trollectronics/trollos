#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include "util/mem.h"
#include "util/log.h"
#include "process.h"
#include "mmu.h"
#include "user.h"
#include "kernel.h"

static Process *_process[MAX_PROCESSES];
static pid_t _process_current;
static pid_t _process_last = -1;

static bool kill_callback(pid_t owner, void *args) {
	Process *p = args;
	_process[owner]->state = PROCESS_STATE_RUNNING;
	return true;
}

pid_t process_create(uid_t uid, gid_t gid) {
	Process *proc;
	pid_t pid, tried = _process_last;
	int i;
	
	do {
		pid = (++_process_last) % MAX_PROCESSES;
		if(pid == tried)
			return -EAGAIN;
	} while(_process[pid]);
	
	
	if(!(proc = kmalloc(sizeof(Process))))
		return -ENOMEM;
	
	proc->pid = pid;
	proc->state = PROCESS_STATE_RUNNING;
	proc->user = uid;
	proc->group = gid;
	//TODO: system_get_ticks()
	proc->time_started = 1337;
	mmu_init_userspace(&proc->userspace);
	
	for(i = 0; i < MAX_PROCESS_FILES; i++)
		proc->file[i] = -1;
	
	_process[pid] = proc;
	return pid;
}

void process_exit(pid_t pid, int return_value) {
	ProcessCallback **callback, *tmp;
	Process *proc;
	pid_t next;
	int i;
	
	if(pid < 0 || pid >= MAX_PROCESSES)
		return;
	if(!_process[pid])
		return;
	
	if(pid == 0)
		panic("Attempted to kill scheduler");
	if(pid == 1)
		panic("Attempted to kill init");
	
	proc = _process[pid];
	for(callback = &proc->callback; *callback; callback = &((*callback)->next)) {
		if((*callback)->func((*callback)->owner, proc)) {
			tmp = *callback;
			*callback = tmp->next;
			kfree(tmp);
		}
	}
	
	if(_process_current == pid) {
		for(next = (pid + 1) % MAX_PROCESSES; !_process[next]; next = (next + 1) % MAX_PROCESSES);
		process_switch_to(next);
	}
	
	for(i = 0; i < MAX_PROCESS_FILES; i++) {
		//TODO: close files
		//file_close(process[pid]->file[i]);
	}
	
	mmu_free_userspace(&_process[pid]->userspace);
	_process[pid]->return_value = return_value;
	_process[pid]->state = PROCESS_STATE_ZOMBIE;
}

void process_set_pc(pid_t pid, void *pc) {
	if(pid < 0 || pid >= MAX_PROCESSES)
		return;
	if(!_process[pid])
		return;
	
	_process[pid]->reg.pc = pc;
}

int process_wait(pid_t pid) {
	ProcessCallback *callback;
	int return_value;
	
	if(pid < 0 || pid >= MAX_PROCESSES)
		return -EINVAL;
	if(!_process[pid])
		return -ESRCH;
	
	if(_process[pid]->state == PROCESS_STATE_ZOMBIE) {
		return_value = _process[pid]->return_value;
		kfree(_process[pid]);
		_process[pid] = NULL;
		return return_value;
	}
	
	callback = kmalloc(sizeof(ProcessCallback));
	callback->func = kill_callback;
	callback->owner = _process_current;
	callback->args = NULL;
	callback->next = _process[pid]->callback;
	_process[pid]->callback = callback;
	_process[_process_current]->state = PROCESS_STATE_BLOCKED;
	
	return -1000;
}

void process_switch_to(pid_t pid) {
	mmu_switch_userspace(&_process[pid]->userspace);
	mmu_invalidate_all();
	_process_current = pid;
}

pid_t process_current() {
	return _process_current;
}

Process *process_from_pid(pid_t pid) {
	if(pid < 0 || pid >= MAX_PROCESSES)
		return NULL;
	return _process[pid];
}

void *scheduler(uint32_t status_reg, void *stack_pointer, void *program_counter, void *regs_tmp) {
	pid_t next;
	_process[_process_current]->reg.pc = program_counter;
	_process[_process_current]->reg.stack = stack_pointer;
	_process[_process_current]->reg.status = status_reg & 0xFFFF;
	memcpy(_process[_process_current]->reg.general, regs_tmp, 4*15);
	
	for(next = (_process_current + 1) % MAX_PROCESSES; !_process[next]; next = (next + 1) % MAX_PROCESSES);
	process_switch_to(next);
	kprintf(LOG_LEVEL_DEBUG, "Scheduler switching to %i\n", next);
	/*kprintf(LOG_LEVEL_DEBUG, " - PC = 0x%x\n", _process[_process_current]->reg.pc);
	kprintf(LOG_LEVEL_DEBUG, " - FL = 0x%x\n", _process[_process_current]->reg.status);
	kprintf(LOG_LEVEL_DEBUG, " - SP = 0x%x\n", _process[_process_current]->reg.stack);*/
	return &_process[next]->reg;
}
