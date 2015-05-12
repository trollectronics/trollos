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

typedef struct Process Process;
struct Process {
	pid_t pid;
	MmuRegRootPointer page_table;
	void *program_counter;
	void *stack_pointer;
	uint32_t status_reg;
	uid_t user;
	gid_t group;
	time_t time_started;
	uint64_t file_bitmap;
	uint32_t file[MAX_PROCESS_FILES]; /*each entry contains an index in ye big olde file table*/
};

void process_jump(void *start);

#endif
