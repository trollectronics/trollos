#ifndef CONSOLE_H_

#include <stdint.h>
#include <device.h>

typedef enum TerminalColor TerminalColor;
enum TerminalColor {
	CONSOLE_COLOR_BLACK,
	CONSOLE_COLOR_BLUE,
	CONSOLE_COLOR_GREEN,
	CONSOLE_COLOR_CYAN,
	CONSOLE_COLOR_RED,
	CONSOLE_COLOR_MAGENTA,
	CONSOLE_COLOR_BROWN,
	CONSOLE_COLOR_LIGHT_GRAY,
	CONSOLE_COLOR_GRAY,
	CONSOLE_COLOR_LIGHT_BLUE,
	CONSOLE_COLOR_LIGHT_GREEN,
	CONSOLE_COLOR_LIGHT_CYAN,
	CONSOLE_COLOR_LIGHT_RED,
	CONSOLE_COLOR_LIGHT_MAGENTA,
	CONSOLE_COLOR_YELLOW,
	CONSOLE_COLOR_WHITE,
};

typedef uint32_t color_t;

typedef enum ConsoleClearDirection ConsoleClearDirection;
enum ConsoleClearDirection {
	CONSOLE_CLEAR_DIRECTION_FORWARD,
	CONSOLE_CLEAR_DIRECTION_BACKWARD,
	CONSOLE_CLEAR_DIRECTION_BOTH,
};

typedef struct ConsoleBackend ConsoleBackend;
struct ConsoleBackend {
	void (*putc)(int c, color_t col_fg, color_t col_bg);
	void (*set_cursor)(unsigned int x, unsigned int y);
	void (*get_cursor)(unsigned int *x, unsigned int *y);
	void (*clear_lines)(ConsoleClearDirection dir);
	void (*clear_on_line)(ConsoleClearDirection dir);
	void (*scroll_up)(unsigned int lines);
	void (*scroll_down)(unsigned int lines);
};

extern CharDev console;

#endif
