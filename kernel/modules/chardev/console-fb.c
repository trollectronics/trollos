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
};

static void _clear() {
	int i;
	for(i = 0; i < _fbcon.w_pixels*_fbcon.h_pixels; i++) {
		_fbcon.fb_mem[i] = 0;
	}
}

static void _scroll_down() {
	uint32_t *fb_mem = _fbcon.fb_mem;
	int i, j;

	for (i = 0; i < (_fbcon.h_chars - 1) * _fbcon.h_font; i++)
		for (j = 0; j < _fbcon.w_pixels/4; j++)
			fb_mem[i * _fbcon.w_pixels/4 + j] = fb_mem[(i + _fbcon.h_font) * _fbcon.w_pixels/4 + j];
	for (i = (_fbcon.h_chars - 1) * _fbcon.h_font; i < _fbcon.h_chars * _fbcon.h_font; i++)
		for (j = 0; j < _fbcon.w_pixels/4; j++)
			fb_mem[i * _fbcon.w_pixels/4 + j] = 0;
	return;
}


static void _putc_raw(int c, uint32_t fg, uint32_t bg) {
	int i, j, row, col;
	uint32_t data;
	
	col = _fbcon.cursor_x * _fbcon.w_font;
	row = _fbcon.cursor_y * _fbcon.h_font;
	
	for (i = 0; i < _fbcon.h_font; i++) {
		data = _fbcon.font[c * _fbcon.h_font + i];
		for (j = 0; j < _fbcon.w_font; j++) {
			_fbcon.fb_mem[(row + i) * _fbcon.w_pixels + col + j] = (data & 1) ? fg : bg;
			data >>= 1;
		}

	}

	_fbcon.cursor_x++;
	if (_fbcon.cursor_x >= _fbcon.w_chars) {
		_fbcon.cursor_y++;
		_fbcon.cursor_x = 0;
	}
	
	if (_fbcon.cursor_y >= _fbcon.h_chars) {
		_scroll_down();
		_fbcon.cursor_y = _fbcon.h_chars - 1;
	}
}


static void _putc(int c, color_t fg, color_t bg) {
	if (c == '\n') {
		_fbcon.cursor_x = 0;
		_fbcon.cursor_y++;
		
		if (_fbcon.cursor_y == _fbcon.h_chars) {
			_scroll_down();
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
	_fbcon.cursor_x = y % (_fbcon.h_chars + 1);
}

static void _get_cursor(unsigned int *x, unsigned int *y) {
	*x = _fbcon.cursor_x;
	*y = _fbcon.cursor_x;
}

void console_fb_init(void *fb_mem, unsigned int w, unsigned int h, void *font, unsigned int font_w, unsigned int font_h) {
	_fbcon.fb_mem = fb_mem;
	_fbcon.w_pixels = w;
	_fbcon.h_pixels = h;
	_fbcon.font = font;
	_fbcon.w_font = font_w;
	_fbcon.h_font = font_h;
	
	_fbcon.w_chars = w/font_w;
	_fbcon.h_chars = h/font_h;
}

ConsoleBackend console_fb = {
	.putc = _putc,
	.set_cursor = _set_cursor,
	.get_cursor = _get_cursor,
};
