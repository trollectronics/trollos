#include <chipset.h>
#include <mem_addr.h>
#include "int.h"
#include "printf.h"
#include "kernel.h"

void **int_vector = (void *) MEM_LLRAM;

void int_init() {
	int i;
	
	int_move_vector((void *) MEM_LLRAM);
	printf("interrupt vector is now at 0x%X\n", int_vector);
	
	for (i = 2; i < 15; i++)
		int_vector[i] = int_stub_bus_error;
	
	int_vector[KERNEL_INTERRUPT] = int_print_shit;
	
	int_vector[CHIPSET_INT_BASE + CHIPSET_INT_NUM_VGA_VSYNC] = int_stub;
	int_enable();
	*CHIPSET_IO(CHIPSET_IO_PORT_INTERRUPT_ENABLE) = 1;
}

void int_stub_handle() {
	
	*CHIPSET_IO(CHIPSET_IO_PORT_IRQ_ACK_VGA) = 0;
}
