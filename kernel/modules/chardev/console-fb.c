#include "console.h"
#include "console-fb.h"

struct {
	volatile uint8_t *fb_mem;
	uint8_t *font;
	uint32_t w_pixels;
	uint32_t h_pixels;
	uint32_t w_chars;
	uint32_t h_chars;
	uint32_t w_font;
	uint32_t h_font;
	
	unsigned int cursor_x;
	unsigned int cursor_y;
	uint8_t cmap[16];
	
} static _fbcon = {
	.fb_mem = NULL,
	.font = NULL,
	.w_pixels = 800,
	.h_pixels = 480,
	.w_chars = 100,
	.h_chars = 30,
	.w_font = 8,
	.h_font = 16,
	
	.cursor_x = 0,
	.cursor_y = 0,
	.cmap = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
};

static void _clear_lines(ConsoleClearDirection dir) {
	unsigned int line = _fbcon.cursor_y;
	unsigned int i, start = 0, end = _fbcon.w_pixels*_fbcon.h_pixels;
	
	if(dir == CONSOLE_CLEAR_DIRECTION_FORWARD)
		start = line * _fbcon.h_font * _fbcon.w_pixels;
	else if(dir == CONSOLE_CLEAR_DIRECTION_BACKWARD)
		end = line * _fbcon.h_font * _fbcon.w_pixels;
	
	for(i = start; i < end; i++) {
		_fbcon.fb_mem[i] = 0;
	}
}

static void _clear_on_line(ConsoleClearDirection dir) {
	//unsigned int col = _fbcon.cursor_x;
	//TODO: stub
}

static void _scroll_down(unsigned int lines) {
	uint32_t *fb_mem = (uint32_t *) _fbcon.fb_mem;
	unsigned int i;
	unsigned int offset = lines * _fbcon.h_font * _fbcon.w_pixels / 4;

	for(i = _fbcon.w_pixels*_fbcon.h_pixels / 4 - 1; i >= offset; i--)
		fb_mem[i] = fb_mem[i - offset];
	
	for(i = 0; i < offset; i++)
		fb_mem[i] = 0;
}

static void _scroll_up(unsigned int lines) {
	uint32_t *fb_mem = (uint32_t *) _fbcon.fb_mem;
	unsigned int i;
	unsigned int offset = lines * _fbcon.h_font * _fbcon.w_pixels / 4;

	for(i = offset; i < _fbcon.w_pixels*_fbcon.h_pixels / 4; i++)
		fb_mem[i - offset] = fb_mem[i];
	
	for(i = _fbcon.w_pixels*_fbcon.h_pixels / 4 - offset; i < _fbcon.w_pixels*_fbcon.h_pixels / 4; i++)
		fb_mem[i] = 0;
}

static void _putc_raw(int c, uint32_t fg, uint32_t bg) {
	int i, j, row, col;
	uint32_t data;
	
	col = _fbcon.cursor_x * _fbcon.w_font;
	row = _fbcon.cursor_y * _fbcon.h_font;
	
	for (i = 0; i < _fbcon.h_font; i++) {
		data = _fbcon.font[c * _fbcon.h_font + i];
		for (j = 0; j < _fbcon.w_font; j++) {
			_fbcon.fb_mem[(row + i) * _fbcon.w_pixels + col + j] = (data & 1) ? _fbcon.cmap[fg] : _fbcon.cmap[bg];
			data >>= 1;
		}

	}

	_fbcon.cursor_x++;
	if (_fbcon.cursor_x >= _fbcon.w_chars) {
		_fbcon.cursor_y++;
		_fbcon.cursor_x = 0;
	}
	
	if (_fbcon.cursor_y >= _fbcon.h_chars) {
		_scroll_up(1);
		_fbcon.cursor_y = _fbcon.h_chars - 1;
	}
}


static void _putc(int c, color_t fg, color_t bg) {
	if (c == '\n') {
		_fbcon.cursor_x = 0;
		_fbcon.cursor_y++;
		
		if (_fbcon.cursor_y == _fbcon.h_chars) {
			_scroll_up(1);
			_fbcon.cursor_y--;
		}
		return;
	}

	if (c == '\r') {
		_fbcon.cursor_x = 0;
		return;
	}

	_putc_raw(c, fg, bg);
	return;
}

static void _set_cursor(unsigned int x, unsigned int y) {
	_fbcon.cursor_x = x % (_fbcon.w_chars + 1);
	_fbcon.cursor_y = y % (_fbcon.h_chars + 1);
}

static void _get_cursor(unsigned int *x, unsigned int *y) {
	*x = _fbcon.cursor_x;
	*y = _fbcon.cursor_y;
}

void console_fb_init(void *fb_mem, unsigned int w, unsigned int h, void *font, unsigned int font_w, unsigned int font_h, uint8_t *cmap) {
	_fbcon.fb_mem = fb_mem;
	_fbcon.w_pixels = w;
	_fbcon.h_pixels = h;
	_fbcon.font = font;
	_fbcon.w_font = font_w;
	_fbcon.h_font = font_h;
	
	_fbcon.w_chars = w/font_w;
	_fbcon.h_chars = h/font_h;
	
	if(cmap) {
		int i;
		for(i = 0; i < 16; i++)
			_fbcon.cmap[i] = cmap[i];
	}
}

ConsoleBackend console_fb = {
	.putc = _putc,
	.set_cursor = _set_cursor,
	.get_cursor = _get_cursor,
	.clear_lines = _clear_lines,
	.clear_on_line = _clear_on_line,
	.scroll_up = _scroll_up,
	.scroll_down = _scroll_down,
};
