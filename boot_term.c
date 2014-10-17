#include "boot_term.h"
#include "vgafont.h"
#include "mem_addr.h"

#define TERM_W		71
#define	TERM_H		30

static int pos_x;
static int pos_y;


void term_init() {
	int i;
	unsigned int *vgabuff = MEM_VGA_RAM;

	pos_x = 0;
	pos_y = 0;
	
	for (i = 0; i < 640*480/4; i++)
		vgabuff[i] = 0;

	/* TODO: Copy palette */
	return;
}


void term_putc(int c, int color) {
	int i, j;
	int row, col, data;
	unsigned char *vgabuff = MEM_VGA_RAM;

	col = pos_x * 9;
	row = pos_y * 16;

	for (i = 0; i < 16; i++) {
		data = vgafont_data[c * 16 + i];
		for (j = 0; j < 8; j++) {
			vgabuff[(row + i) * 640 + col + j] = (data & 1) ? color : 0;
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
