#include <chipset.h>
#include <mem_addr.h>
#include <syscall.h>
#include <errno.h>
#include "util/log.h"
#include "util/mem.h"
#include "kernel.h"
#include "int.h"

void **int_vector = (void *) 0xDDD00;

typedef struct IntHandler IntHandler;
struct IntHandler {
	void (*isr)(uint32_t interrupt, void *data);
	void *data;

	IntHandler *next;
};


IntHandler *int_handler[INTERRUPTS];


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
	
	int_vector[CHIPSET_INT_BASE + 1] = int_stub;
	int_enable();
	//*CHIPSET_IO(CHIPSET_IO_PORT_INTERRUPT_ENABLE) = 1;
}

void int_set_handler(uint32_t i, void *handler) {
	int_vector[i] = handler;
}

void int_perihperal_enable(uint32_t n) {
	*CHIPSET_IO(32 + n) = 0x0;
	*CHIPSET_IO(n) = 0x1; //priority
}

int int_isr_register(uint32_t i, void (*isr)(uint32_t, void *), void *data) {
	IntHandler *handler = NULL;

	if (!(handler = kmalloc(sizeof(IntHandler))))
		goto fail;


	handler->isr = isr;
	handler->data = data;
	handler->next = int_handler[i];
	int_handler[i] = handler;

	return 0;
	
	fail:
	return -ENOMEM;
}

void int_handler_internal(uint32_t interrupt) {
	IntHandler *handler = NULL;

	for (handler = int_handler[interrupt]; handler; handler = handler->next) {
		//TODO: check return value, skip rest?
		handler->isr(interrupt, handler->data);
	}

	//TODO: run scheduler here?
	//Should ISRs be able to set a flag somewhere that scheduler needs to run
}


// TODO: move
void int_vga_handle() {
	*CHIPSET_IO(32 + CHIPSET_INT_NUM_VGA) = 0x0;
}
