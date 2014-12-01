#include <stdint.h>
#include "boot_term.h"
#include "vgafont.h"
#include "mem_addr.h"
#include "bios_info.h"

#define TERM_W		71
#define	TERM_H		30

static int pos_x;
static int pos_y;
static uint32_t vsync_count;

void boot_term_vsync() {
	vsync_count++;
	BOOT_TERM_VSYNC_ACK;
	return;
}


uint32_t boot_term_get_vsync() {
	return vsync_count;
}


void term_init() {
	int i;
	volatile uint32_t *vgabuff = MEM_VGA_RAM;
	volatile uint16_t *vgapal = MEM_PAL_RAM;

	pos_x = 0;
	pos_y = 0;
	
	for (i = 0; i < 640*480/4; i++)
		vgabuff[i] = 0;
	
	vgapal[0] = 0;
	vgapal[1] = 0x14;
	vgapal[2] = 0x540;
	vgapal[3] = 0x554;
	vgapal[4] = 0xA000;
	vgapal[5] = 0xA014;
	vgapal[6] = 0xA380;
	vgapal[7] = 0xA554;
	vgapal[8] = 0x52AA;
	vgapal[9] = 0x52BF;
	vgapal[10] = 0x57EA;
	vgapal[11] = 0x57FF;
	vgapal[12] = 0xFAAA;
	vgapal[13] = 0xFAAF;
	vgapal[14] = 0xFFFA;
	vgapal[15] = 0xFFFF;

	BOOT_TERM_VSYNC_ENABLE;
	
	return;
}


void term_putc(int c, int color) {
	int i, j;
	int row, col, data;
	volatile uint8_t *vgabuff = MEM_VGA_RAM;

	col = pos_x * 9;
	row = pos_y * 16;

	for (i = 0; i < 16; i++) {
		data = vgafont_data[c * 16 + i];
		for (j = 0; j < 8; j++) {
			vgabuff[(row + i) * 800 + col + j] = (data & 1) ? color : 0;
			data >>= 1;
		}
	}

	pos_x++;
	if (pos_x == TERM_W)
		pos_x = 0, pos_y++;
	if (pos_y == TERM_H)
		/* TODO: Implement scrolling */
		pos_y = TERM_H - 1;
	return;
}


void term_putc_term(unsigned char c, int color) {
	if (c == '\n') {
		pos_y++;
		pos_x = 0;
		if (pos_y == TERM_H)
			/* TODO: Implement scrolling */
			pos_y--;
	} else if (c == '\r')
		pos_x = 0;
	else
		term_putc(c, color);
}


void term_puts(char *str, int color) {
	for (; *str; str++)
		term_putc_term((unsigned) *str, color);
	return;
}


void term_export() {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;

	bi->font = vgafont_data;
	bi->term_x = pos_x;
	bi->term_y = pos_y;
	bi->def_fg = 15;
	bi->def_bg = 0;

	return;
}


void term_import() {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	
	pos_x = bi->term_x;
	pos_y = bi->term_y;
	
	return;
}
