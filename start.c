/* No other function than _start in this file. Makes sure _start is at the top	*
** of the built binary								*/

#include "test.h"
#include "boot_term.h"
#include "util.h"

void _start() {
	memset((void *) BSS, 0, 32*1024);
	term_init();

	term_puts("TrollBook(tm) BIOS Version 0.17 (C) Trollectronics 2014\n", 15);
	test();
	for(;;);
}
