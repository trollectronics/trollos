#include "boot_term.h"
#include "romfs.h"

int test() {
	struct RomfsFileDescriptor desc;

	if (!romfs_detect((void *) 0x2000))
		term_puts("Bad magic in RomFS\n", 12);
	else {
		desc = romfs_locate((void *) 0x2000, "arne");
		if (!desc.filename)
			term_puts("Couldn't find file /arne", 12);
		else {
			term_puts("Found file /arne!", 10);
			term_puts(" Contents:\n", 15);
			term_puts(desc.data, 15);
		}
	}

	for (;;);
}
