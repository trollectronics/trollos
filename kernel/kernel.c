#include "terminal.h"
#include "printf.h"
#include "int.h"

int ostkaka = 42;

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
	
	
	for (;;);
	int_init();

	return 42;
}
