#include <stdint.h>
#include <chipset.h>
#include "debug.h"

void debug_put_byte(uint8_t byte) {
	*CHIPSET_IO(CHIPSET_IO_PORT_DEBUG) = byte;
}

uint8_t debug_get_byte() {
	return *CHIPSET_IO(CHIPSET_IO_PORT_DEBUG);
}
