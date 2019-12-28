#include <syscall.h>
#include <chipset.h>
#include <module.h>
#include <fcntl.h>
#include <trollos/vfs.h>
#include "modules/binformat/elf.h"
#include "modules/blockdev/memblk.h"
#include "util/log.h"
#include "util/kconsole.h"
#include "util/mem.h"
#include "util/printf.h"
#include "util/string.h"
#include "int.h"
#include "mmu.h"
#include <trollos/process.h>
#include "kernel.h"
#include "debug.h"
#include "init.h"
#include "root.h"

FileModuleMap filetable[MAX_TOTAL_FILES];

void panic(const char *message) {
	int_disable();
	//terminal_set_bg(CONSOLE_COLOR_BLACK);
	//terminal_set_fg(CONSOLE_COLOR_LIGHT_RED);
	//terminal_set_pos(0, 29);
	kconsole_write("*** KERNEL PANIC: ", 18);
	kconsole_write(message, strlen(message));
	kconsole_write(" ***", 4);
	for(;;);
}

int main(int argc, char **argv) {
	int (*init)(int argc, char **argv);
	int i;
	Device *console;
	
	console = early_init();
	
	kprintf(LOG_LEVEL_NONE, "\x1b[2J\x1b[1;1H\x1b[0mTrollOS kernel\n");
	log_set_level(LOG_LEVEL_INFO);
	
	kprintf(LOG_LEVEL_NONE, "argv = { ");
	if(argc > 0) {
		kprintf(LOG_LEVEL_NONE, "%s", argv[0]);
	}
	for(i = 1; i < argc; i++) {
		kprintf(LOG_LEVEL_NONE, ", ");
		kprintf(LOG_LEVEL_NONE, "%s", argv[i]);
	}
	kprintf(LOG_LEVEL_NONE, " }\n");

	for(i = 0; i < argc; i++) {
		if(!strncmp(argv[i], "loglevel=", 9)) {
			log_set_level(str_parse_int(argv[i] + 9));
			break;
		}
	}
	
	mmu_init();
	mmu_print_status();
	kprintf(LOG_LEVEL_INFO, "Kernel heap is at 0x%X\n", ksbrk(0));
	int_init();
	
	for(i = 0; i < argc; i++) {
		if(!strcmp(argv[i], "debug")) {
			set_debug_traps();
			breakpoint();
			break;
		}
	}
	
	dev_t console_devnum;
	device_register("console", console, &console_devnum);
	//stdin, stdout, stderr
	vfs_open_device(console_devnum, 0);
	
	Device *memblk_dev;
	memblk_init();
	memblk_open(__bin_os_romfs, __bin_os_romfs_len);
	dev_t memblk_devnum = device_lookup_name("memblk0", &memblk_dev);
	
	if(fs_romfs_instantiate(memblk_devnum) >= 0)
		kprintf(LOG_LEVEL_INFO, "Found romfs filesysem on memblk0\n");
	
	int fd;
	fd = vfs_open("/sbin/init", 0);
	kprintf(LOG_LEVEL_INFO, "fd %i\n", fd);
	vfs_seek(fd, 0, SEEK_END);
	size_t init_size = vfs_tell(fd);
	vfs_seek(fd, 0, SEEK_SET);
	void *init_elf = kmalloc(init_size);
	vfs_read(fd, init_elf, init_size);

	pid_t pid;
	Process *proc;
	pid = process_create(0, 0);
	kprintf(LOG_LEVEL_DEBUG, "process created\n");
	process_switch_to(pid);
	process_set_pc(pid, init = elf_load(init_elf));

	kfree(init_elf);

	proc = process_from_pid(pid);
	
	//stdin, stdout, stderr
	proc->file[0] = 0;
	proc->file[1] = 0;
	proc->file[2] = 0;
	
	kprintf(LOG_LEVEL_INFO, "Now starting init @ 0x%X\n", init);
	int_set_handler(CHIPSET_INT_BASE + 1, process_isr);
	int_disable();
	int_perihperal_enable(CHIPSET_INT_NUM_VGA);
	
	process_jump(init);
	
	panic("kernel main() has returned");
	return 1;
}
