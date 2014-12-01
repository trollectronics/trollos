#include "boot_term.h"
#include "romfs.h"
#include "elf.h"
#include "sd.h"

int test() {
	struct RomfsFileDescriptor desc;
	char *argv[] = { "osloader.elf", "arne" };
	int i;
	void *go;

	term_puts("Init SD-card\n", 10);
	
	if (!romfs_detect((void *) 0x10000))
		term_puts("Bad magic in RomFS\n", 12);
	else {
		desc = romfs_locate((void *) 0x10000, "/boot/osloader.elf");
		if (!desc.filename)
			term_puts("Couldn't find file /boot/osloader.elf", 12);
		else {
			term_puts("Attempting to launch /boot/osloader.elf...\n", 10);
			//elf_run(desc.data, 2, argv);
			mmu_init();
			go = elf_load(desc.data);
			printf("phys of entry 0x%X is 0x%X\n", go, mmu_get_physical(go));
			term_export();
			mmu_enable_and_jump(go);
			for(;;);
			term_puts("Returned from elf_load()\n", 15);
		}
	}

	for (;;) {
		for (i = 0; i < 1000000; i++);
		term_putc(',', 4);
	}
}
