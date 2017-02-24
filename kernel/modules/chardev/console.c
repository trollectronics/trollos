#include <device.h>
#include "../../util/string.h"
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
	ParseState parse_state;
	ConsoleBackend *backend;
	uint32_t color_fg;
	uint32_t color_bg;
} static _console = {
	.parse_state = PARSE_STATE_ASCII,
	.backend = NULL,
	.color_fg = CONSOLE_COLOR_LIGHT_GRAY,
	.color_bg = CONSOLE_COLOR_BLACK,
};

void _csi(char cmd, int argc, int *arg) {
	switch(cmd) {
		case 'A':
		case 'B':
		case 'C':
		case 'D':
			break;
		
		case 'm':
			break;
	}
}

ssize_t _write(const void *buf, size_t count) {
	char *s = buf;
	char c;
	size_t i;
	int arg[10];
	int argc;
	int a;
	
	for(i = 0; i < count; i++) {
		c = *s++;
		//check for null byte?
		
		retry:
		switch(_console.parse_state) {
			case PARSE_STATE_ASCII:
				if(c == 27) {
					_console.parse_state = PARSE_STATE_ESC;
					break;
				}
				
				_console.backend->putc(c, _console.color_fg, _console.color_bg);
				break;
			case PARSE_STATE_ESC:
				if(c == '[') {
					_console.parse_state = PARSE_STATE_CSI;
					argc = 0;
					a = 0;
				} else
					_console.parse_state = PARSE_STATE_ASCII;
				break;
			case PARSE_STATE_CSI:
				if(isdigit(c)) {
					_console.parse_state = PARSE_STATE_CSI_ARG;
					goto retry;
				} else if(c == ';') {
					_console.parse_state = PARSE_STATE_CSI_ARG;
				} else if(isalpha(c)) {
					_csi(c, argc, arg);
					_console.parse_state = PARSE_STATE_ASCII;
				}
				break;
			case PARSE_STATE_CSI_ARG:
				if(isdigit(c)) {
					a *= 10;
					a += c - '0';
				} else {
					arg[argc] = a;
					a = 0;
					argc++;
					_console.parse_state = PARSE_STATE_CSI;
					goto retry;
				}
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
