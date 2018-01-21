#include <stdint.h>
#include <bios_info.h>
#include "modules/chardev/console.h"
#include "modules/chardev/console-fb.h"
#include "util/kconsole.h"

Device console_dev;

Device *early_init() {
	uint8_t cmap[] = {
		0, 4, 2, 6, 1, 4, 3, 7,
		0 + 8, 4 + 8, 2 + 8, 6 + 8, 1 + 8, 4 + 8, 3 + 8, 7 + 8,
	};
	console_fb_init(MEM_VGA_RAM, 800, 480, BIOS_INFO_ADDR->font, 8, 16, cmap);
	console_init(&console_fb);
	kconsole_init(&console);
	
	console_dev.type = DEVICE_TYPE_CHAR;
	console_dev.chardev = console;
	
	return &console_dev;
}
