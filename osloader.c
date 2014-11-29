#include "terminal.h"
#include "printf.h"

int _start(int argc, char **argv) {
	terminal_set_bg(TERMINAL_COLOR_MAGENTA);
	terminal_set_fg(TERMINAL_COLOR_CYAN);
	terminal_puts("Hello from osloader.elf!\n");
	printf("Arne %.8i '%s'\n", 42, "lol");
	int_init();
	for (;;);

	return 42;
}
