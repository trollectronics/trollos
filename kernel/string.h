#ifndef __KINC_STRING_H__
#define	__KINC_STRING_H__

#include <stdint.h>

uint32_t strnlen(const char *c, uint32_t maxlen);
char *strncpy(char *dest, const char *src, uint32_t max);
int strcmp(const char *s1, const char *s2);
char *strchr(char *str, char chr);
char *strtok_r(char *buff, const char *delim, char **next);
uint32_t str_parse_int(const char *str);

#endif
