.text
buss_error_text: .asciz "*** KERNEL PANIC: bus error ***"

indicate_bus_error:
	move.l #0x81800, %a0
	move.l #(640*20)-1, %d0
1:
	eori.b #4, (%a0)+
	dbra %d0, 1b
	rts

.global int_stub_bus_error
int_stub_bus_error:
	move.l %a0, -(%sp)
	move.l %a1, -(%sp)
	move.l %d0, -(%sp)
	move.l %d1, -(%sp)
	link %fp, #0
	
	move.l buss_error_text, -(%sp)
	#jsr.l terminal_puts
	jsr.l indicate_bus_error
	addq.l #2, %sp
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
