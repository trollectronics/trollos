.text

.global int_stub
int_stub:
	jsr.l int_stub_handle
	rte

.global int_enable
int_enable:
	mov.w %sr, %d0
	ori.w #0x3000, %d0
	mov.w %d0, %sr
	rts
