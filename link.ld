_stack = 0xF8000;

MEMORY {
	rom(rx): ORIGIN = 0x0, LENGTH = 512k
	sram(rwx): ORIGIN = 0x80000, LENGTH = 512k
}

SECTIONS {
	.text 0x0 : {
		KEEP(*(.vector))
		KEEP(*(.start))
		*(.text*)
		*(.rodata*)
		*(.data*)
	} > rom
	
	.bss 0xEA000 : {
		__bss_start = .;
		*(.bss*)
		*(COMMON)
		__bss_end = .;
		
	} > sram
}
