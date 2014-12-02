#include <stdint.h>

int streq(const char *s1, const char *s2, int limit) {
	int i;

	for (i = 0; i < limit && s1[i] && s2[i] && s1[i] == s2[i]; i++);
	if (i == limit)
		return 1;
	return s1[i] == s2[i];
}


int strlen(const char *s) {
	int i;

	for (i = 0; s[i]; i++);
	return i;
}
