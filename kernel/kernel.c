#include <syscall.h>
#include "kernel.h"
#include "terminal.h"
#include "module.h"
#include "memblk.h"
#include "printf.h"
#include "string.h"
#include "log.h"
#include "int.h"
#include "mmu.h"
#include "mem.h"
#include "process.h"
#include "elf.h"

FileModuleMap filetable[MAX_TOTAL_FILES];

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
	int (*init)(int argc, char **argv);
	int i;
	
	terminal_set_bg(TERMINAL_COLOR_BLACK);
	terminal_set_fg(TERMINAL_COLOR_LIGHT_BLUE);
	terminal_puts("\nTrollOS kernel\n");
	terminal_set_fg(TERMINAL_COLOR_LIGHT_GRAY);

	terminal_puts("argv = { ");
	if(argv > 0) {
		terminal_set_fg(TERMINAL_COLOR_LIGHT_CYAN);
		printf("%s", argv[0]);
	}
	for(i = 1; i < argc; i++) {
		terminal_set_fg(TERMINAL_COLOR_LIGHT_GRAY);
		printf(", ");
		terminal_set_fg(TERMINAL_COLOR_LIGHT_CYAN);
		printf("%s", argv[i]);
	}
	terminal_set_fg(TERMINAL_COLOR_LIGHT_GRAY);
	terminal_puts(" }\n");
	/*if (argc >= 3)
		memdev_from_arg(argv[2]);*/
	
	for(i = 1; i < argc; i++) {
		if(!strncmp(argv[i], "loglevel=", 9)) {
			log_set_level(str_parse_int(argv[i] + 9));
			break;
		}
	}
	
	module_init();

	mmu_init();
	mmu_print_status();
	kprintf(LOG_LEVEL_INFO, "Kernel heap is at 0x%X\n", ksbrk(0));
	int_init();
	
	mmu_init_userspace();
	init = elf_load(argv[3]);
	
	kprintf(LOG_LEVEL_INFO, "Now starting init @ 0x%X\n", init);
	process_jump(init);
	//generate bus error
	terminal_set_fg(TERMINAL_COLOR_YELLOW);
	printf("I will now generate a buss error.");
	i = *((int *) 0xDEADBEEF);
	printf("lalala %i\n", i);
	for (;;);

	return 42;
}
