#include <syscall.h>
#include <chipset.h>
#include "modules/binformat/elf.h"
#include "modules/blockdev/memblk.h"
#include "modules/chardev/terminal.h"
#include "modules/module.h"
#include "util/log.h"
#include "util/mem.h"
#include "util/printf.h"
#include "util/string.h"
#include "int.h"
#include "mmu.h"
#include "process.h"
#include "kernel.h"
#include "debug.h"
#include "init.h"

FileModuleMap filetable[MAX_TOTAL_FILES];

void panic(const char *message) {
	int_disable();
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
	
	terminal_clear();
	terminal_set_bg(TERMINAL_COLOR_BLACK);
	terminal_set_fg(TERMINAL_COLOR_LIGHT_BLUE);
	terminal_puts("\nTrollOS kernel\n");
	terminal_set_fg(TERMINAL_COLOR_LIGHT_GRAY);
	
	log_set_level(LOG_LEVEL_INFO);
	
	terminal_puts("argv = { ");
	if(argc > 0) {
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
	
	pid_t pid;
	pid = process_create(0, 0);
	kprintf(LOG_LEVEL_DEBUG, "process created\n");
	process_switch_to(pid);
	process_set_pc(pid, init = elf_load(bin_init));
	
	kprintf(LOG_LEVEL_INFO, "Now starting init @ 0x%X\n", init);
	int_set_handler(CHIPSET_INT_BASE + 1, process_isr);
	int_disable();
	int_perihperal_enable(CHIPSET_INT_NUM_VGA);
	
	process_jump(init);
	
	panic("kernel main() has returned");
	return 1;
}
