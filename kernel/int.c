#include <chipset.h>
#include <mem_addr.h>
#include <syscall.h>
#include "util/log.h"
#include "kernel.h"
#include "int.h"

void **int_vector = (void *) 0xDDD00;

void int_init() {
	int i;
	
	int_move_vector((void *) 0xDDD00);
	//kprintf(LOG_LEVEL_INFO, "Interrupt vector is at 0x%X\n", int_vector);
	
	for(i = 0; i < 255; i++)
		int_vector[INT_OFFSET_TRAP + i] = int_invalid_interrupt;
	
	for (i = 2; i < 15; i++)
		int_vector[i] = int_stub_bus_error;
	
	for(i = 0; i < 16; i++)
		int_vector[INT_OFFSET_TRAP + i] = int_invalid_trap;
	
	int_vector[SYSCALL_TRAP + INT_OFFSET_TRAP] = int_syscall;
	
	int_vector[CHIPSET_INT_BASE + CHIPSET_INT_NUM_VGA_VSYNC] = int_stub;
	int_enable();
	//*CHIPSET_IO(CHIPSET_IO_PORT_INTERRUPT_ENABLE) = 1;
}

void int_set_handler(uint32_t i, void *handler) {
	int_vector[i] = handler;
}

void int_vga_handle() {
	
	*CHIPSET_IO(CHIPSET_IO_PORT_IRQ_ACK_VGA) = 0;
}
