#include <stdint.h>

<<<<<<< HEAD
void *memset(void *s, int c, unsigned int n) {
	uint8_t *dest = s;
	
	for(; n; n--)
		*(dest++) = c;
	
	return s;
}


void *memcpy(void *dest, void *src, unsigned int n) {
	uint8_t *d = dest, *s = src;
	
	for(; n; n--)
		*d++ = *s++;
	
	return dest;
}

=======
>>>>>>> de43ab126044cc50b787fe1085db3ea50c70f4ce

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
