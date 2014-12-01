.text

.global boot_term_vsync_stub
boot_term_vsync_stub:
	jsr.l boot_term_vsync
	rte
