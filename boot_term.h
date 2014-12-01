#ifndef __BOOT_TERM_H__
#define	__BOOT_TERM_H__

#include <stdint.h>
#include "mem_addr.h"

#define	BOOT_TERM_VSYNC_ENABLE	(*((unsigned int *) (MEM_CHIPSET_SPACE + 0x0)) = 1)
#define	BOOT_TERM_VSYNC_ACK	(*((unsigned int *) (MEM_CHIPSET_SPACE + 0x4)) = 0)

uint32_t boot_term_get_vsync();
void term_init();
void term_putc(int c, int color);
void term_putc_term(unsigned char c, int color);
void term_puts(char *str, int color);

void term_export();
void term_import();

#endif
