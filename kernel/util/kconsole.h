#ifndef UTIL_KCONSOLE_H_
#define UTIL_KCONSOLE_H_

#include <device.h>

int kconsole_write(const void *buf, size_t count);
void kconsole_init(CharDev *chr);

#endif
