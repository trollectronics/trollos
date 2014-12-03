#include "string.h"
#include <mem_addr.h>

uint32_t strnlen(const char *c, uint32_t maxlen) {
	uint32_t i;

	for (i = 0; i < maxlen && c[i]; i++);
	return i;
}


char *strncpy(char *dest, const char *src, uint32_t max) {
	uint32_t i;

	for (i = 0; i < max && src[i]; i++)
		dest[i] = src[i];
	if (i == max)
		dest[i - 1] = 0;
	else
		dest[i] = 0;
	return dest;
}


int strcmp(const char *s1, const char *s2) {
	int i;
	for (i = 0; s1[i] && s2[i] && s1[i] == s2[i]; i++);
	if (s1[i] != s2[i])
		return ((signed) s1) - ((signed) s2);
	return 0;
}


char *strchr(char *str, char chr) {
	while (*str++)
		if (*str == chr)
			return str;
	return NULL;
}


char *strtok_r(char *buff, const char *delim, char **next) {
	char *search, *ret, *new;
	
	search = buff?buff:(*next);
	ret = NULL;
	
	while (*delim++) {
		new = strchr(search, *delim);
		if (!ret || ret > new)
			ret = new;
	}

	*ret = 0;
	*next = ret?ret + 1:NULL;
	return search;
}


uint32_t str_parse_int(const char *str) {
	uint32_t ret = 0;
	int i, len;

	if (!(len = strnlen(str, 32)))
		return 0;
	if (*str != '0') {
		/* Parse base 10 */
		for (i = 0; i < len; i++) {
			if (str[i] < '0' || str[i] > '9')
				return ret;
			len *= 10;
			len += (str[i] - '0');
		}

		return ret;
	}

	if (str[1] == 'x' || str[1] == 'X') {
		/* Parse hex */
		ret <<= 4;
		for (i = 2; i < len; i++) {
			if (str[i] < '0' || str[i] > '9') {
				if (str[i] < 'a' || str[i] > 'f') {
					if (str[i] < 'A' || str[i] > 'F')
						return 0;
					else
						ret |= (str[i] - 'A' + 0xA);
				} else
					ret |= (str[i] - 'a' + 0xA);
			} else
				ret |= (str[i] - '0');
		}
	} else if (len == 1) {
		return 0;
	} else {
		/* TODO: Decode octal */
		return 0;
	}

	return ret;
}
