#include <stdarg.h>
#include "log.h"
#include "printf.h"
#include "terminal.h"

static const char const *logprefix[] = {
	[LOG_LEVEL_NONE] = "",
	[LOG_LEVEL_CRITICAL] = "CRITICAL",
	[LOG_LEVEL_ERROR] = "ERROR",
	[LOG_LEVEL_WARNING] = "WARN",
	[LOG_LEVEL_INFO] = "INFO",
	[LOG_LEVEL_DEBUG] = "DEBUG",
};

static const TerminalColor logcolor[] = {
	[LOG_LEVEL_NONE] = TERMINAL_COLOR_LIGHT_GRAY,
	[LOG_LEVEL_CRITICAL] = TERMINAL_COLOR_RED,
	[LOG_LEVEL_ERROR] = TERMINAL_COLOR_LIGHT_RED,
	[LOG_LEVEL_WARNING] = TERMINAL_COLOR_YELLOW,
	[LOG_LEVEL_INFO] = TERMINAL_COLOR_WHITE,
	[LOG_LEVEL_DEBUG] = TERMINAL_COLOR_LIGHT_GRAY,
};

static LogLevel loglevel = LOG_LEVEL_DEBUG;

int kprintf(LogLevel level, char *format, ...) {
	va_list va;
	int ret;
	
	if(level > loglevel)
		return -1;
	
	va_start(va, format);
	if(level < LOG_LEVELS) {
		terminal_set_fg(TERMINAL_COLOR_LIGHT_GRAY);
		printf("[");
		terminal_set_fg(logcolor[level]);
		printf("%s", logprefix[level]);
		terminal_set_fg(TERMINAL_COLOR_LIGHT_GRAY);
		printf("] ");
	}
	ret = vprintf(format, va);
	va_end(va);
	return ret;
}

void log_set_level(LogLevel level) {
	if(level < LOG_LEVELS)
		loglevel = level;
}
