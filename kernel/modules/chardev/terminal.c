#include "terminal.h"
#include <bios_info.h>
#include <mem_addr.h>

struct {
	uint32_t w_pixels;
	uint32_t h_pixels;
	uint32_t w_chars;
	uint32_t h_chars;
	uint32_t w_font;
	uint32_t h_font;
} static _terminal = {
	.w_pixels = 800,
	.h_pixels = 480,
	.w_chars = 100,
	.h_chars = 30,
	.w_font = 8,
	.h_font = 16,
};

/* TODO: Implement cursor */

void terminal_clear() {
	volatile unsigned char *vgabuff = MEM_VGA_RAM;
	int i;
	for(i = 0; i < _terminal.w_pixels*_terminal.h_pixels; i++) {
		vgabuff[i] = 0;
	}
}

void terminal_scroll() {
	volatile unsigned int *vgabuff = MEM_VGA_RAM;
	int i, j;

	for (i = 0; i < (_terminal.h_chars - 1) * _terminal.h_font; i++)
		for (j = 0; j < _terminal.w_pixels/4; j++)
			vgabuff[i * _terminal.w_pixels/4 + j] = vgabuff[(i + _terminal.h_font) * _terminal.w_pixels/4 + j];
	for (i = (_terminal.h_chars - 1) * _terminal.h_font; i < _terminal.h_chars * _terminal.h_font; i++)
		for (j = 0; j < _terminal.w_pixels/4; j++)
			vgabuff[i * _terminal.w_pixels/4 + j] = 0;
	return;
}


void terminal_putc(int c, int fg, int bg) {
	int i, j, row, col, data;
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	volatile unsigned char *vgabuff = MEM_VGA_RAM;

	col = bi->term_x * _terminal.w_font;
	row = bi->term_y * _terminal.h_font;
	
	for (i = 0; i < _terminal.h_font; i++) {
		data = bi->font[c * _terminal.h_font + i];
		for (j = 0; j < _terminal.w_font; j++) {
			vgabuff[(row + i) * _terminal.w_pixels + col + j] = (data & 1) ? fg : bg;
			data >>= 1;
		}

	}

	bi->term_x++;
	if (bi->term_x >= _terminal.w_chars)
		bi->term_y++, bi->term_x = 0;
	if (bi->term_y >= _terminal.h_chars)
		terminal_scroll(), bi->term_y = _terminal.h_chars - 1;
	return;
}


void terminal_putc_ctrl(int c, int fg, int bg) {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	
	if (c == '\n') {
		bi->term_x = 0, bi->term_y++;
		if (bi->term_y == _terminal.h_chars)
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


void terminal_set_pos(int x, int y) {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	
	bi->term_x = x % (_terminal.w_chars + 1);
	bi->term_y = y % (_terminal.h_chars + 1);
}


void terminal_putc_simple(char c) {
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	unsigned char uc = (unsigned char) c;

	terminal_putc_ctrl(uc, bi->def_fg, bi->def_bg);
	return;
}


void terminal_put_counted(const char *str, int count) {
	unsigned char *ustr = (void *) str;
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;
	
	for (; count; terminal_putc_ctrl(*ustr, bi->def_fg, bi->def_bg), ustr++, count--);
	return;
}


void terminal_puts(const char *str) {
	unsigned char *ustr = (void *) str;
	volatile struct BiosInfo *bi = BIOS_INFO_ADDR;

	for (; *ustr; terminal_putc_ctrl(*ustr, bi->def_fg, bi->def_bg), ustr++);
	return;
}

int terminal_write(int pid, int id, void *ptr, uint32_t count) {
	terminal_put_counted(ptr, count);
}
