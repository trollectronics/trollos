/* No other function than _start in this file. Makes sure _start is at the top	*
** of the built binary								*/

#include "test.h"
#include "boot_term.h"

void _start() {
	term_init();

	test();
	for(;;);
}
