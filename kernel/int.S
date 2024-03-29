.text
buss_error_text: .asciz "bus error"
invalid_trap: .asciz "Caught invalid trap\n"
invalid_interrupt: .asciz "Caught invalid interrupt\n"

.align 2
.global int_stub_bus_error
int_stub_bus_error:
	move.l %a0, -(%sp)
	move.l %a1, -(%sp)
	move.l %d0, -(%sp)
	move.l %d1, -(%sp)
	link %fp, #0
	
	pea buss_error_text
	jsr.l panic
	adda.l #4, %sp
1:
	bra 1b
	
	unlk %fp
	move.l (%sp)+, %d1
	move.l (%sp)+, %d0
	move.l (%sp)+, %a1
	move.l (%sp)+, %a0
	rte

.align 2
.global int_stub
int_stub:
	move.l %a0, -(%sp)
	move.l %a1, -(%sp)
	move.l %d0, -(%sp)
	move.l %d1, -(%sp)
	
	move.w 22(%fp), %d0 //22 ?!?!
	andi.l #0x0FFF, %d0
	move.l %d0, -(%sp)
	jsr.l int_handler_internal
	adda.l #4, %sp
	
	move.l (%sp)+, %d1
	move.l (%sp)+, %d0
	move.l (%sp)+, %a1
	move.l (%sp)+, %a0
	rte


arne: .asciz "syscall %u with %u %u %u %u %u\n"

.align 2
.global int_invalid_trap
int_invalid_trap:
	or.w #0x0700, %sr
	move.l %a0, -(%sp)
	move.l %a1, -(%sp)
	move.l %d0, -(%sp)
	move.l %d1, -(%sp)
	link %fp, #0
	
	pea invalid_trap
	move.l #3, -(%sp)
	jsr.l kprintf
	adda.l #8, %sp
	
	#jsr.l breakpoint
	
	unlk %fp
	move.l (%sp)+, %d1
	move.l (%sp)+, %d0
	move.l (%sp)+, %a1
	move.l (%sp)+, %a0
	rte

.align 2
.global int_invalid_interrupt
int_invalid_interrupt:
	move.l %a0, -(%sp)
	move.l %a1, -(%sp)
	move.l %d0, -(%sp)
	move.l %d1, -(%sp)
	link %fp, #0
	
	pea invalid_interrupt
	jsr.l panic
	adda.l #4, %sp
1:
	bra 1b
	
	unlk %fp
	move.l (%sp)+, %d1
	move.l (%sp)+, %d0
	move.l (%sp)+, %a1
	move.l (%sp)+, %a0
	rte

.align 2
.global int_move_vector
int_move_vector:
	link %fp, #0
	move.l 8(%fp), %a0
	move.l %a0, %d1
	
|	movec %vbr, %a1
|	
|	move.l #31, %d0
|1:
|	move.l (%a1)+, (%a0)+
|	move.l (%a1)+, (%a0)+
|	move.l (%a1)+, (%a0)+
|	move.l (%a1)+, (%a0)+
|	
|	move.l (%a1)+, (%a0)+
|	move.l (%a1)+, (%a0)+
|	move.l (%a1)+, (%a0)+
|	move.l (%a1)+, (%a0)+
|	dbra %d0, 1b
	
	movec %d1, %vbr
	unlk %fp
	rts

.align 2
.global trap
trap:
	link %fp, #0
	move.l 8(%fp), %d0
	
	trap #5
	
	unlk %fp
	rts

.align 2
.global int_enable
int_enable:
	and.w #0xF8FF, %sr
	#mov.w #0x2000, %sr
	rts

.align 2
.global int_disable
int_disable:
	or.w #0x0700, %sr
	rts
