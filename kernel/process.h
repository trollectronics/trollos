#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdint.h>
#include <stdbool.h>
#include <mmu.h>
#include "user.h"
#include "time.h"

typedef int32_t pid_t;

#define MAX_PROCESS_FILES 64
#define MAX_PROCESSES 1024

typedef enum ProcessState ProcessState;
enum ProcessState {
	PROCESS_STATE_RUNNING,
	PROCESS_STATE_BLOCKED,
	PROCESS_STATE_ZOMBIE,
};

typedef struct Process Process;
struct Process {
	pid_t pid;
	ProcessState state;
	int return_value;
	MmuRegRootPointer page_table;
	uid_t user;
	gid_t group;
	time_t time_started;
	int32_t file[MAX_PROCESS_FILES]; /*each entry contains an index in ye big olde file table*/
	struct {
		uint32_t status;
		void *pc;
		void *stack;
		uint32_t general[15];
#if 0
		uint8_t fpu[184];
#endif
	} reg;
};

void process_jump(void *start);
void process_switch_to(pid_t pid);
void process_exit(pid_t pid, int return_value);
pid_t process_create(uid_t uid, gid_t gid);
pid_t process_current();
Process *process_from_pid(pid_t pid);
void *process_isr();

#endif
