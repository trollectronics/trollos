#include "kconsole.h"

static CharDev *kconsole;

int kconsole_write(const void *buf, size_t count) {
	if(kconsole)
		return kconsole->write(buf, count);
	return -1;
}

void kconsole_init(CharDev *chr) {
	kconsole = chr;
}
