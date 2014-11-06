.text
.org 0x100
.global int_stub

int_stub:
	jsr.l int_stub_handle
	rte

.global int_enable
int_enable:
#	mov.w 0x3000, %sr
	rts
