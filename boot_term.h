#ifndef __BOOT_TERM_H__
#define	__BOOT_TERM_H__

void term_init();
void term_putc(int c, int color);
void term_putc_term(unsigned char c, int color);
void term_puts(char *str, int color);

void term_export();
void term_import();

#endif
