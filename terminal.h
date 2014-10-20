#ifndef __TERMINAL_H__
#define	__TERMINAL_H__

enum TerminalColor {
	TerminalColorBlack		= 0,
	TerminalColorBlue		= 1,
	TerminalColorGreen		= 2,
	TerminalColorCyan		= 3,
	TerminalColorRed		= 4,
	TerminalColorMagenta		= 5,
	TerminalColorBrown		= 6,
	TerminalColorLightGray		= 7,
	TerminalColorGray		= 8,
	TerminalColorLightBlue		= 9,
	TerminalColorLightGreen		= 10,
	TerminalColorLightCyan		= 11,
	TerminalColorLightRed		= 12,
	TerminalColorLightMagenta	= 13,
	TerminalColorYellow		= 14,
	TerminalColorWhite		= 15,
};

void terminal_puts(char *str);
void terminal_putc_simple(char c);
void terminal_putc_ctrl(int c, int fg, int bg);
void terminal_put_counted(char *str, int count);
void terminal_set_fg(enum TerminalColor color);
void terminal_set_bg(enum TerminalColor color);

#endif
