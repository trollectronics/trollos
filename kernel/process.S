.section .bss
.align 2
.global regs_tmp
regs_tmp:
	.space 256

.section .text
.align 2
.global process_isr
process_isr:
	movem.l %a0-%a6/%d0-%d7, (regs_tmp)
	link %fp, #0

	#set up userspace stack frame with stack pointer, return address, etc
	
	jsr.l int_vga_handle
	
	pea regs_tmp
	move.l 6(%fp), -(%sp)
	move.l %usp, %a0
	move.l %a0, -(%sp)
	move.w 4(%fp), %d0
	andi.l #0xFFFF, %d0
	move.l %a0, -(%sp)
	
	jsr.l scheduler
	adda.l #16, %sp
	
	
	move.l (%d0), %a0
	move.w %a0, 4(%fp)
	move.l 4(%d0), 6(%fp)
	
	move.l 8(%d0), %a0
	move.l %a0, %usp
	
	unlk %fp
	movem.l 12(%d0), %a0-%a6/%d0-%d7
	rte

arnetext:	.asciz "dest=0x%X"
.align 2
.global process_jump
process_jump:
	move.l 4(%sp), %a0

	move.l #0, %d0
	movec.l %d0, %usp
	move.w #0, -(%sp)
	move.l %a0, -(%sp)
	move.w #0, -(%sp)
	rte
