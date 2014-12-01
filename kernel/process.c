#include <stdint.h>
#include <stdbool.h>
#include "process.h"

uint32_t process_bitmap[MAX_PROCESSES/32];
Process *process[MAX_PROCESSES];

void scheduler() {
	static uint32_t current_process;
	uint32_t i, j, bits;
	
	for(i = ((current_process + 1) % MAX_PROCESSES)/(MAX_PROCESSES/32); !process_bitmap[i]; i = (i + 1) % (MAX_PROCESSES/32));
	bits = process_bitmap[i];
	for(j = 0; !(bits & 0x1); bits >>=1, j++);
	
	current_process = i*32 + j;
	//TODO: load crp with process page table
	//TODO: set up stack pointers, load registers
	//TODO: run process
}
