#include <stdbool.h>
#include <device.h>
#include "../../util/string.h"
#include "../../util/printf.h"
#include "console.h"

#define MAX_ARG 10;

typedef enum ParseState ParseState;
enum ParseState {
	PARSE_STATE_ASCII,
	PARSE_STATE_ESC,
	PARSE_STATE_CSI,
	PARSE_STATE_CSI_ARG,
};

struct {
	struct {
		ParseState state;
		int arg[10];
		int argc;
		int a;
	} parse_state;
	ConsoleBackend *backend;
	bool bold;
	color_t color_fg;
	color_t color_bg;
	struct {
		unsigned int x;
		unsigned int y;
	} saved_cursor;
} static _console = {
	.parse_state = {PARSE_STATE_ASCII},
	.backend = NULL,
	.bold = false,
	.color_fg = CONSOLE_COLOR_LIGHT_GRAY,
	.color_bg = CONSOLE_COLOR_BLACK,
	.saved_cursor = {0, 0},
};

void _csi(char cmd, int argc, int *arg) {
	int i;
	unsigned int x, y;
	int delta;
	
	//printf("\n                                    \r%c %i %i %i\n", cmd, argc, arg[0], arg[1]);
	
	delta = argc > 0 && arg[0] > 0 ? arg[0] : 1;
	_console.backend->get_cursor(&x, &y);
	
	switch(cmd) {
		/* Relative movement */
		case 'A':
			if(y)
				_console.backend->set_cursor(x, y - delta);
			break;
		case 'B':
			_console.backend->set_cursor(x, y + delta);
			break;
		case 'C':
			_console.backend->set_cursor(x + delta, y);
			break;
		case 'D':
			if(x)
				_console.backend->set_cursor(x - delta, y);
			break;
		
		/* Line movement */
		case 'E':
			_console.backend->set_cursor(0, y + delta);
			break;
		case 'F':
			if(!y)
				y = 1;
			_console.backend->set_cursor(0, y - delta);
			break;
		
		/* Absolute movement */
		case 'G':
			_console.backend->set_cursor(delta - 1, y);
			break;
		case 'f':
		case 'H':
			if(argc == 0)
				_console.backend->set_cursor(1, 1);
			if(argc == 1)
				_console.backend->set_cursor(x, delta - 1);
			else
				_console.backend->set_cursor(arg[1] > 0 ? arg[1] - 1 : 0, delta - 1);
			break;
		
		/* Erasing */
		case 'J':
			if(argc == 0 || arg[0] == 0) {
				_console.backend->clear_lines(CONSOLE_CLEAR_DIRECTION_FORWARD);
			} else if(arg[0] == 1) {
				_console.backend->clear_lines(CONSOLE_CLEAR_DIRECTION_BACKWARD);
			} else {
				_console.backend->clear_lines(CONSOLE_CLEAR_DIRECTION_BOTH);
			}
			break;
		case 'K':
			if(argc == 0 || arg[0] == 0) {
				_console.backend->clear_on_line(CONSOLE_CLEAR_DIRECTION_FORWARD);
			} else if(arg[0] == 1) {
				_console.backend->clear_on_line(CONSOLE_CLEAR_DIRECTION_BACKWARD);
			} else {
				_console.backend->clear_on_line(CONSOLE_CLEAR_DIRECTION_BOTH);
			}
			break;
		
		/* Scrolling */
		case 'S':
			_console.backend->scroll_up(delta);
			break;
		case 'T':
			_console.backend->scroll_down(delta);
			break;
		
		/* Saving/restoring */
		case 's':
			_console.saved_cursor.x = x;
			_console.saved_cursor.y = y;
			break;
		case 'u':
			_console.backend->set_cursor(_console.saved_cursor.x, _console.saved_cursor.y);
			break;
		
		/* Attributes */
		case 'm':
			if(!argc) {
				_console.color_fg = CONSOLE_COLOR_LIGHT_GRAY;
				_console.color_bg = CONSOLE_COLOR_BLACK;
				_console.bold = false;
				break;
			}
			
			for(i = 0; i < argc; i++) {
				if(arg[i] == 0) {
					_console.color_fg = CONSOLE_COLOR_LIGHT_GRAY;
					_console.color_bg = CONSOLE_COLOR_BLACK;
					_console.bold = false;
				} else if(arg[i] == 1) {
					_console.bold = true;
				} else if(arg[i] == 22) {
					_console.bold = false;
				} else if(arg[i] > 30 && arg[i] < 38) {
					_console.color_fg = arg[i] - 30;
				} else if(arg[i] == 39) {
					_console.color_fg = CONSOLE_COLOR_LIGHT_GRAY;
				} else if(arg[i] > 40 && arg[i] < 48) {
					_console.color_bg = arg[i] - 40;
				} else if(arg[i] == 49) {
					_console.color_bg = CONSOLE_COLOR_BLACK;
				}
			}
				
			break;
	}
}

ssize_t _write(const void *buf, size_t count) {
	const char *s = buf;
	char c;
	size_t i;
	
	for(i = 0; i < count; i++) {
		c = *s++;
		//check for null byte?
		
		switch(_console.parse_state.state) {
			case PARSE_STATE_ASCII:
				if(c == 27) {
					_console.parse_state.state = PARSE_STATE_ESC;
					break;
				}
				
				_console.backend->putc(c, _console.color_fg + (_console.bold ? 8 : 0), _console.color_bg);
				break;
			case PARSE_STATE_ESC:
				if(c == '[') {
					_console.parse_state.state = PARSE_STATE_CSI;
					_console.parse_state.argc = 0;
					_console.parse_state.a = 0;
				} else
					_console.parse_state.state = PARSE_STATE_ASCII;
				break;
			case PARSE_STATE_CSI:
				if(c == ';') {
					_console.parse_state.arg[_console.parse_state.argc] = -1;
					if(_console.parse_state.argc < 10)
						_console.parse_state.argc++;
					_console.parse_state.a = 0;
					
					break;
				} else if(!isdigit(c)) {
					_console.parse_state.arg[_console.parse_state.argc] = -1;
					if(_console.parse_state.argc < 10)
						_console.parse_state.argc++;
					_console.parse_state.a = 0;
					
					_console.parse_state.state = PARSE_STATE_ASCII;
					_csi(c, _console.parse_state.argc, _console.parse_state.arg);
					break;
				}
				
				_console.parse_state.state  = PARSE_STATE_CSI_ARG;
			case PARSE_STATE_CSI_ARG:
				if(c == ';') {
					_console.parse_state.arg[_console.parse_state.argc] = _console.parse_state.a;
					if(_console.parse_state.argc < 10)
						_console.parse_state.argc++;
					_console.parse_state.a = 0;
					_console.parse_state.state  = PARSE_STATE_CSI;
					break;
				} else if(!isdigit(c)) {
					_console.parse_state.arg[_console.parse_state.argc] = _console.parse_state.a;
					if(_console.parse_state.argc < 10)
						_console.parse_state.argc++;
					_console.parse_state.a = 0;
					
					_console.parse_state.state = PARSE_STATE_ASCII;
					_csi(c, _console.parse_state.argc, _console.parse_state.arg);
					break;
				}
				
				_console.parse_state.a *= 10;
				_console.parse_state.a += c - '0';
				break;
		}
	}
	
	return (ssize_t) count;
}

void console_init(ConsoleBackend *backend) {
	_console.backend = backend;
};

CharDev console = {
	.read = NULL,
	.write = _write,
	.ioctl = NULL,
};
