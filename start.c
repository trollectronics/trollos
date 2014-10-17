/* No other function than _start in this file. Makes sure _start is at the top	*
** of the built binary								*/

#include "test.h"

void _start() {
	unsigned int *ptr = 0x80800;
	int i;

	for (i = 0; i < 640*480/8; i++) {
		*ptr = 0x01020304, ptr++;
		*ptr = 0x01010101, ptr++;
	}

	test();
	for(;;);
}
