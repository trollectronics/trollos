.text
init_text: .asciz "I am init\n"

.align 2
.global _start
_start:
	#try invalid trap
	trap #5
	
	move.l #0xFFFF, %d0
1:
	subi.l #1, %d0
	bne 1b

	#exit
	move.l #5, %a4
	move.l #4, %a3
	move.l #3, %a2
	move.l #2, %a1
	move.l #1, %a0
	move.l #0, %d0
	trap #10
