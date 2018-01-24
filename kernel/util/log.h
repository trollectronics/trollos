#ifndef LOG_H_
#define LOG_H_
#include "printf.h"

typedef enum LogLevel LogLevel;
enum LogLevel {
	LOG_LEVEL_NONE,
	LOG_LEVEL_CRITICAL,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_SPAM,
	LOG_LEVELS
};

int kprintf(LogLevel level, char *format, ...);
void log_set_level(LogLevel level);

#endif
