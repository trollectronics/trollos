#include "test.h"
#include "boot_term.h"
#include "util.h"
#include "printf.h"

void _start();
void mmu_int_stub();
void _stack();

extern long __bss_start;
extern long __bss_end;

const void const *vector[256] __attribute__ ((section (".vector"))) = {
	[0] = _stack,
	[1] = _start,
	[2 ... 15] = mmu_int_stub,
};

void main() {
	memset(&__bss_start, 0, &__bss_end - &__bss_start);
	term_init();

	term_puts("TrollBook(tm) BIOS Version 0.17 (C) Trollectronics 2014\n", 15);
	test();
	for(;;);
}
