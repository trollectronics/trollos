#include <stdint.h>
#include <chipset.h>
#include "debug.h"
#include <nop.h>

void debug_put_byte(uint8_t byte) {
	nop();
	*CHIPSET_IO(CHIPSET_IO_PORT_DEBUG) = byte;
	nop();
}

uint8_t debug_get_byte() {
	uint8_t byte;
	nop();
	byte = *CHIPSET_IO(CHIPSET_IO_PORT_DEBUG);
	nop();
	return byte;
}
