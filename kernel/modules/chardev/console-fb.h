#ifndef CONSOLE_FB_H_
#define	CONSOLE_FB_H_

#include <stdint.h>

extern ConsoleBackend console_fb;

void console_fb_init(void *fb_mem, unsigned int w, unsigned int h, void *font, unsigned int font_w, unsigned int font_h);

#endif
