#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>

void debug_put_byte(uint8_t byte);
uint8_t debug_get_byte();

void set_debug_traps();
void breakpoint();

#endif
