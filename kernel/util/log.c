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

static const char const *logcolor[] = {
	[LOG_LEVEL_NONE] = "\x1b[0m",
	[LOG_LEVEL_CRITICAL] = "\x1b[31m",
	[LOG_LEVEL_ERROR] = "\x1b[1;31m",
	[LOG_LEVEL_WARNING] = "\x1b[1;33m",
	[LOG_LEVEL_INFO] = "\x1b[1;37m",
	[LOG_LEVEL_DEBUG] = "\x1b[37m",
};

static LogLevel loglevel = LOG_LEVEL_DEBUG;

int kprintf(LogLevel level, char *format, ...) {
	va_list va;
	int ret;
	
	if(level > loglevel)
		return -1;
	
	va_start(va, format);
	if(level < LOG_LEVELS && level != LOG_LEVEL_NONE) {
		printf("\x1b[0m" "[" "%s%s" "\x1b[0m" "] ", logcolor[level], logprefix[level]);
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
