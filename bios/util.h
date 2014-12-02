#ifndef __UTIL_H__
#define	__UTIL_H__

#ifndef NULL
#define	NULL ((void *) 0)
#endif

int streq(const char *s1, const char *s2, int limit);
int strlen(const char *s);
void *memset(void *s, int c, unsigned int n);
void *memcpy(void *dest, void *src, unsigned int n);

#endif
