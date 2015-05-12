#include <stdint.h>
#include "elf.h"
#include "boot_term.h"
#include "romfs.h"
#include "sd.h"
#include "mmu.h"
#include "printf.h"

int test() {
	struct RomfsFileDescriptor desc;
	char *argv[] = { "kernel.elf", "loglevel=5", "newdev=memdev,addr=0x10000,size=0x70000", NULL};
	int i;
	void *go;

	term_puts("Init SD-card\n", 10);
	sd_init();
	
	if (!romfs_detect((void *) 0x10000))
		term_puts("Bad magic in RomFS\n", 12);
	else {
		desc = romfs_locate((void *) 0x10000, "/boot/kernel.elf");
		if (!desc.filename)
			term_puts("Couldn't find file /boot/kernel.elf", 12);
		else {
			term_puts("Attempting to launch /boot/kernel.elf...\n", 10);
			mmu_init();
			go = elf_load(desc.data);
			printf(" -> Entry is 0x%X, mapped to physical address 0x%X\n", go, mmu_get_physical(go));
			desc = romfs_locate((void *) 0x10000, "/sbin/init");
			argv[3] = desc.data;
			term_export();
			mmu_enable_and_jump(go, 3, argv);
			for(;;);
			term_puts("Returned from elf_load()\n", 15);
		}
	}

	for (;;) {
		for (i = 0; i < 1000000; i++);
		term_putc(',', 4);
	}
}
