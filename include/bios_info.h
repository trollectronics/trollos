#ifndef __INCLUDE_BIOS_INFO_H__
#define	__INCLUDE_BIOS_INFO_H__

#include "mem_addr.h"

typedef struct BiosInfo BiosInfo;
struct BiosInfo {
	unsigned char *font;
	int term_x;
	int term_y;
	int def_fg;
	int def_bg;

	int			vsync_clock;
};

#define BIOS_INFO_ADDR ((volatile BiosInfo *) 0xDDC00)

#endif
