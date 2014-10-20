#include "terminal.h"
#include "printf.h"

int _start(int argc, char **argv) {
	terminal_set_bg(TerminalColorMagenta);
	terminal_set_fg(TerminalColorCyan);
	terminal_puts("Hello from osloader.elf!\n");
	printf("Arne %.8i '%s'\n", 42, "lol");

	return 42;
}
