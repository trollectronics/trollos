#include "terminal.h"
#include <bootimg/bios_info.h>
#include <bootimg/mem_addr.h>


#define	TERM_W			71
#define	TERM_H			30

/* TODO: Implement cursor */

void terminal_scroll() {
	volatile unsigned int *vgabuff = MEM_VGA_RAM;
	int i, j;

	for (i = 0; i < (TERM_H - 1) * 16; i++)
		for (j = 0; j < 800/4; j++)
			vgabuff[i * 800/4 + j] = vgabuff[(i + 16) * 800/4 + j];
	for (; i < TERM_H * 16; i++)
		for (; j < 800/4; j++)
			vgabuff[i * 800/4 + j] = 0;
	return;
}


void terminal_putc(int c, int fg, int bg) {
	int i, j, row, col, data;
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	volatile unsigned char *vgabuff = MEM_VGA_RAM;

	col = bi->term_x * 9;
	row = bi->term_y * 16;
	
	for (i = 0; i < 16; i++) {
		data = bi->font[c * 16 + i];
		for (j = 0; j < 9; j++) {
			vgabuff[(row + i) * 800 + col + j] = (data & 1) ? fg : bg;
			data >>= 1;
		}

	}

	bi->term_x++;
	if (bi->term_x >= TERM_W)
		bi->term_y++, bi->term_x = 0;
	if (bi->term_y >= TERM_H)
		terminal_scroll(), bi->term_y = TERM_H - 1;
	return;
}


void terminal_putc_ctrl(int c, int fg, int bg) {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	
	if (c == '\n') {
		bi->term_x = 0, bi->term_y++;
		if (bi->term_y == TERM_H)
			terminal_scroll(), bi->term_y--;
		return;
	}

	if (c == '\r') {
		bi->term_x = 0;
		return;
	}

	terminal_putc(c, fg, bg);
	return;
}

void terminal_set_fg(enum TerminalColor color) {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	
	bi->def_fg = color;
	return;
}


void terminal_set_bg(enum TerminalColor color) {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	
	bi->def_bg = color;
	return;
}


void terminal_putc_simple(char c) {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	unsigned char uc = (unsigned char) c;

	terminal_putc_ctrl(uc, bi->def_fg, bi->def_bg);
	return;
}


void terminal_put_counted(char *str, int count) {
	unsigned char *ustr = (void *) str;
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	
	for (; count; terminal_putc_ctrl(*ustr, bi->def_fg, bi->def_bg), ustr++, count--);
	return;
}


void terminal_puts(char *str) {
	unsigned char *ustr = (void *) str;
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;

	for (; *ustr; terminal_putc_ctrl(*ustr, bi->def_fg, bi->def_bg), ustr++);
	return;
}
