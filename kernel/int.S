.text
buss_error_text: .asciz "bus error"

.global int_stub_bus_error
int_stub_bus_error:
	move.l %a0, -(%sp)
	move.l %a1, -(%sp)
	move.l %d0, -(%sp)
	move.l %d1, -(%sp)
	link %fp, #0
	
	pea buss_error_text
	jsr.l panic
	addq.l #4, %sp
1:
	bra 1b
	
	unlk %fp
	move.l (%sp)+, %d1
	move.l (%sp)+, %d0
	move.l (%sp)+, %a1
	move.l (%sp)+, %a0
	rte

.global int_stub
int_stub:
	move.l %a0, -(%sp)
	move.l %a1, -(%sp)
	move.l %d0, -(%sp)
	move.l %d1, -(%sp)
	
	jsr.l int_stub_handle
	
	move.l (%sp)+, %d1
	move.l (%sp)+, %d0
	move.l (%sp)+, %a1
	move.l (%sp)+, %a0
	rte

.global int_move_vector
int_move_vector:
	link %fp, #0
	move.l 8(%fp), %a0
	move.l %a0, %d1
	
	movec %vbr, %a1
	
	move.l #31, %d0
1:
	move.l (%a1)+, (%a0)+
	move.l (%a1)+, (%a0)+
	move.l (%a1)+, (%a0)+
	move.l (%a1)+, (%a0)+
	
	move.l (%a1)+, (%a0)+
	move.l (%a1)+, (%a0)+
	move.l (%a1)+, (%a0)+
	move.l (%a1)+, (%a0)+
	dbra %d0, 1b
	
	movec %d1, %vbr
	unlk %fp
	rts

.global int_enable
int_enable:
	and.w #0xF8FF, %sr
	#mov.w #0x2000, %sr
	rts
