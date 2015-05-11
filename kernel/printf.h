#ifndef __PRINTF_H__
#define	__PRINTF_H__

typedef enum LogLevel {
	LOG_LEVEL_NONE,
	LOG_LEVEL_CRITICAL,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVELS
};

#define	kprintf(x,...)	printf(__VA_ARGS__)
int printf(char *format, ...);

#endif
