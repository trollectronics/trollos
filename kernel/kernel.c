#include "terminal.h"
#include "memdev.h"
#include "printf.h"
#include "int.h"
#include "mmu.h"
#include "mem.h"

int ostkaka = 42;

void panic(const char *message) {
	terminal_set_bg(TERMINAL_COLOR_BLACK);
	terminal_set_fg(TERMINAL_COLOR_LIGHT_RED);
	terminal_set_pos(0, 29);
	terminal_puts("*** KERNEL PANIC: ");
	terminal_puts(message);
	terminal_puts(" ***");
	for(;;);
}

int main(int argc, char **argv) {
	int i;
	
	terminal_set_bg(TERMINAL_COLOR_BLACK);
	terminal_set_fg(TERMINAL_COLOR_CYAN);
	terminal_puts("\nHello, MMU-world!\n");
	terminal_set_fg(TERMINAL_COLOR_LIGHT_GRAY);
	
	terminal_puts("argv = { ");
	if(argv > 0)
		printf("%s ", argv[0]);
	for(i = 1; i < argc; i++)
		printf(", %s ", argv[i]);
	terminal_puts("}\n");
	/*if (argc >= 3)
		memdev_from_arg(argv[2]);*/
	
	mmu_init();
	mmu_print_status();
	printf("Heap is at 0x%X\n", ksbrk(0));
	printf("Heap is at 0x%X\n", ksbrk(MMU_PAGE_SIZE));
	uint32_t *arne = ksbrk(0);
	arne--;
	*arne = 0xDEADBEEF;
	printf("I have written this to mem: 0x%X", *arne);
	for(;;);
	int_init();
	//generate bus error
	i = *((int *) 0xDEADBEEF);
	printf("lalala %i\n", i);
	for (;;);

	return 42;
}
