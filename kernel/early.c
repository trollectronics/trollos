#include <bios_info.h>
#include "modules/chardev/console.h"
#include "modules/chardev/console-fb.h"
#include "util/kconsole.h"

void early_init() {
	console_fb_init(MEM_VGA_RAM, 800, 480, BIOS_INFO_ADDR->font, 8, 16);
	console_init(&console_fb);
	kconsole_init(&console);
}
