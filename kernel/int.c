#include <chipset.h>
#include <mem_addr.h>
#include "int.h"
#include "printf.h"

void **int_vector = (void *) MEM_LLRAM;

void int_init() {
	int i;
	volatile unsigned int *io_reg_interrupt = (MEM_CHIPSET_SPACE + 0x0);
	
	int_move_vector((void *) MEM_LLRAM);
	printf("interrupt vector is now at 0x%X\n", int_vector);
	
	for (i = 2; i < 15; i++)
		int_vector[i] = int_stub_bus_error;
	
	int_vector[CHIPSET_INT_BASE + CHIPSET_INT_NUM_VGA_VSYNC] = int_stub;
	int_enable();
	*io_reg_interrupt = 0x1;
}

void int_stub_handle() {
	volatile int *addr = (MEM_CHIPSET_SPACE + 0x4);
	
	*addr = 0x0;
}
