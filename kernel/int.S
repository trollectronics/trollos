.text
buss_error_text: .asciz "bus error"
syscall_text: .asciz "Syscall number %i called\n"
init_died: .asciz "init has died"
init_called_exit: .asciz "Init has called exit()\n"

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
	
	jsr.l int_stub_handle
	
	move.l (%sp)+, %d1
	move.l (%sp)+, %d0
	move.l (%sp)+, %a1
	move.l (%sp)+, %a0
	rte

.align 2
.global int_syscall
int_syscall:
	
	move.l %d0, -(%sp)
	move.l %d0, -(%sp)
	pea syscall_text
	move.l #5, -(%sp)
	jsr.l kprintf
	adda.l #12, %sp
	
	tst (%sp)+
	bne 1f
	
	pea init_called_exit
	move.l #1, -(%sp)
	jsr.l kprintf
	adda.l #8, %sp
	
	pea init_died
	jsr.l panic

1:
	rte

.align 2
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
