.text

.align 2
.global _syscall
_syscall:
	link %fp, #0
	move.l %a2, -(%sp)
	move.l %a3, -(%sp)
	move.l %a4, -(%sp)
	
	move.l 8(%fp), %d0
	move.l 12(%fp), %a0
	move.l 16(%fp), %a1
	move.l 20(%fp), %a2
	move.l 24(%fp), %a3
	move.l 28(%fp), %a4
	trap #10
	
	move.l (%sp)+, %a4
	move.l (%sp)+, %a3
	move.l (%sp)+, %a2
	unlk %fp
	rts


.align 2
.global int_syscall
int_syscall:
	or.w #0x0700, %sr
	move.l %d1, -(%sp)
	
	move.l %a4, -(%sp)
	move.l %a3, -(%sp)
	move.l %a2, -(%sp)
	move.l %a1, -(%sp)
	move.l %a0, -(%sp)
	move.l %d0, -(%sp)
	jsr.l syscall_stub
	adda.l #24, %sp
	move.l (%sp)+, %d1
	
	cmp  #-1000, %d0
	bne 1f
	jmp process_isr

1:
	rte
