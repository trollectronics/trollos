
/* Non-portable way of confusing GCC's optimizer */
void *memset(void *s, int c, unsigned int n) {
	unsigned char *b = s;
	unsigned int i, z;

	if (!(((unsigned int) s) & 3) && !(n & 3)) {
		n >>= 2;
		z = (c & 0xFF);
		z = (z | (z << 8) | (z << 16) | (z << 24));
		for (i = 0; i < n; i += 2)
			b[i] = z;
		for (i = 1; i < n; i += 2)
			b[i] = z;
		return s;
	}

	for (i = 0; i < n; i += 2)
		b[i] = c;
	for (i = 1; i < n; i += 2)
		b[i] = c;
	return s;
}
