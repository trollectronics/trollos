.text
init_text: .asciz "I am init\n"

.align 2
.global _start
_start:
	#try invalid trap
	trap #5
	
	#exit
	move.l #0, %d0
	trap #10
