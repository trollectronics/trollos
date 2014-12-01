#ifndef __TERMINAL_H__
#define	__TERMINAL_H__

enum TerminalColor {
	TERMINAL_COLOR_BLACK,
	TERMINAL_COLOR_BLUE,
	TERMINAL_COLOR_GREEN,
	TERMINAL_COLOR_CYAN,
	TERMINAL_COLOR_RED,
	TERMINAL_COLOR_MAGENTA,
	TERMINAL_COLOR_BROWN,
	TERMINAL_COLOR_LIGHT_GRAY,
	TERMINAL_COLOR_GRAY,
	TERMINAL_COLOR_LIGHT_BLUE,
	TERMINAL_COLOR_LIGHT_GREEN,
	TERMINAL_COLOR_LIGHT_CYAN,
	TERMINAL_COLOR_LIGHT_RED,
	TERMINAL_COLOR_LIGHT_MAGENTA,
	TERMINAL_COLOR_YELLOW,
	TERMINAL_COLOR_WHITE,
};

void terminal_puts(char *str);
void terminal_putc_simple(char c);
void terminal_putc_ctrl(int c, int fg, int bg);
void terminal_put_counted(char *str, int count);
void terminal_set_fg(enum TerminalColor color);
void terminal_set_bg(enum TerminalColor color);

#endif
