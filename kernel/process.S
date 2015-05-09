.global process_isr
process_isr:
	link %fp, #0
	moves.l %a0, -(%sp)
	moves.l %a1, -(%sp)
	moves.l %d0, -(%sp)
	moves.l %d1, -(%sp)
	
	
	
	#set up userspace stack frame with stack pointer, return address, etc
	
	move.l %usp, %a0
	move.l 6(%fp), -(%sp)
	move.l %d0, -(%sp)
	move.w 4(%fp), -(%sp)
	jsr.l scheduler
	addq.l #6, %sp
	addq.l #6, %sp
	
	move.l 12(%d0), %a0
	move.l %a0, %usp
	
	move.w 18(%a0), (%sp)
	move.l 8(%a0), -2(%sp)
	
	moves.l (%sp)+, %d1
	moves.l (%sp)+, %d0
	moves.l (%sp)+, %a1
	moves.l (%sp)+, %a0
	unlk %fp
	rte

