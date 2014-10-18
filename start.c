/* No other function than _start in this file. Makes sure _start is at the top	*
** of the built binary								*/

#include "test.h"
#include "boot_term.h"

void _start() {
	term_init();

	term_puts("TrollBook(tm) BIOS Version 0.17 (C) Trollectronics 2014\n", 15);
	test();
	for(;;);
}
