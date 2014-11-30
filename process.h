#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdint.h>
#include <stdbool.h>
#include "mmu.h"

#define MAX_PROCESS_FILES 64
#define MAX_PROCESSES 1024

typedef struct Process Process;
struct Process {
	uint32_t id;
	MmuRegRootPointer page_table;
	uint32_t user;
	uint32_t group;
	uint32_t time_started;
	uint64_t file_bitmap;
	uint32_t file[MAX_PROCESS_FILES]; /*each entry contains an index in ye big olde file table*/
};

#endif
