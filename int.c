#include "../src/chipset.h"
#include "../bootimg/mem_addr.h"
#include "printf.h"

extern void *int_stub;
extern void int_enable();

void int_init() {
	void **int_vec = (void **) 0x60;
	unsigned int *io_port = (MEM_CHIPSET_SPACE);
	int i;

	for (i = 0; i < 8; i++)
		int_vec[i] = &int_stub;
	int_vec[ChipsetIntNumVGAVSync] = &int_stub;
	printf("interrupt stub: %X\n", (unsigned int) &int_stub);
	__asm__("mov.w 0x3000, %sr");
	*io_port = 1;
	return;
}

void int_stub_handle() {
	terminal_putc_simple('.');
}
