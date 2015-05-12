#ifndef __KINC_STRING_H__
#define	__KINC_STRING_H__

#include <stdint.h>

#define	STRING_ARG_MAX		512

uint32_t strnlen(const char *c, uint32_t maxlen);
char *strncpy(char *dest, const char *src, uint32_t max);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, uint32_t n);
char *strchr(char *str, char chr);
char *strtok_r(char *buff, const char *delim, char **next);
uint32_t str_parse_int(const char *str);
char *str_extract_arg(const char *str_in, char *findarg, char *argout, uint32_t arglen);

#endif
