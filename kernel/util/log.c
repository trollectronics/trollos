#include <stdarg.h>
#include "../modules/chardev/console.h"
#include "log.h"
#include "printf.h"

static const char const *logprefix[] = {
	[LOG_LEVEL_NONE] = "NONE",
	[LOG_LEVEL_CRITICAL] = "CRIT",
	[LOG_LEVEL_ERROR] = "EROR",
	[LOG_LEVEL_WARNING] = "WARN",
	[LOG_LEVEL_INFO] = "INFO",
	[LOG_LEVEL_DEBUG] = "DBUG",
};

static const TerminalColor logcolor[] = {
	[LOG_LEVEL_NONE] = CONSOLE_COLOR_LIGHT_GRAY,
	[LOG_LEVEL_CRITICAL] = CONSOLE_COLOR_RED,
	[LOG_LEVEL_ERROR] = CONSOLE_COLOR_LIGHT_RED,
	[LOG_LEVEL_WARNING] = CONSOLE_COLOR_YELLOW,
	[LOG_LEVEL_INFO] = CONSOLE_COLOR_WHITE,
	[LOG_LEVEL_DEBUG] = CONSOLE_COLOR_LIGHT_GRAY,
};

static LogLevel loglevel = LOG_LEVEL_DEBUG;

int kprintf(LogLevel level, char *format, ...) {
	va_list va;
	int ret;
	
	if(level > loglevel)
		return -1;
	
	va_start(va, format);
	if(level < LOG_LEVELS) {
		//terminal_set_fg(CONSOLE_COLOR_LIGHT_GRAY);
		printf("[");
		//terminal_set_fg(logcolor[level]);
		printf("%s", logprefix[level]);
		//terminal_set_fg(CONSOLE_COLOR_LIGHT_GRAY);
		printf("] ");
	}
	ret = vprintf(format, va);
	va_end(va);
	return ret;
}

void log_set_level(LogLevel level) {
	if(level < LOG_LEVELS) {
		loglevel = level;
		kprintf(LOG_LEVEL_INFO, "Setting loglevel to %s\n", logprefix[loglevel]);
	}
}
