#include "terminal.h"
#include "printf.h"
#include "int.h"
#include "mmu.h"

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
	
	mmu_init();
	int_init();
	//generate bus error
	i = *((int *) 0xDEADBEEF);
	printf("lalala %i\n", i);
	for (;;);

	return 42;
}
