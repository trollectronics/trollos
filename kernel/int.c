#include <chipset.h>
#include <mem_addr.h>
#include "int.h"
#include "printf.h"

void **int_vector = (void *) MEM_LLRAM;

void int_init() {
	//int i;
	//volatile unsigned int *io_port = (MEM_CHIPSET_SPACE);
	
	int_move_vector((void *) MEM_LLRAM);

	/*for (i = 2; i < 15; i++)
		int_vector[i] = &int_stub_bus_error;*/
	int_vector[CHIPSET_INT_NUM_VGA_VSYNC] = &int_stub;
	printf("interrupt stub: %p\n",  &int_stub);
	int_enable();
	//*io_port = 1;
	return;
}

void int_stub_handle() {
	volatile int *addr = MEM_CHIPSET_SPACE + 0x4;

	//terminal_putc_simple('.');
	*addr = 0;
}
