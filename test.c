#include "boot_term.h"
#include "romfs.h"
#include "elf.h"

int test() {
	struct RomfsFileDescriptor desc;
	char *argv[] = { "osloader.elf", "arne" };
	int i;
	
	if (!romfs_detect((void *) 0x90000))
		term_puts("Bad magic in RomFS\n", 12);
	else {
		desc = romfs_locate((void *) 0x90000, "/boot/osloader.elf");
		if (!desc.filename)
			term_puts("Couldn't find file /boot/osloader.elf", 12);
		else {
			term_puts("Attempting to launch /boot/osloader.elf...\n", 10);
			elf_load(desc.data, 2, argv);
			term_puts("Returned from elf_load()\n", 15);
		}
	}

	for (;;) {
		for (i = 0; i < 1000000; i++);
		term_putc(',', 4);
	}
}
