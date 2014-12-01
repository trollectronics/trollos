#ifndef __BIOS_INFO_H__
#define	__BIOS_INFO_H__

#include "mem_addr.h"

#define	BIOS_INFO_ADDR		((volatile void *) (0x7FF00 + MEM_LLRAM))


struct BiosInfo {
	unsigned char		*font;
	int			term_x;
	int			term_y;
	int			def_fg;
	int			def_bg;

	int			vsync_clock;
};


#endif
