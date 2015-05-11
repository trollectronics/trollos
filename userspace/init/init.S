.text
init_text: .asciz "I am init\n"

.align 2
.global _start
_start:
	move.l #0, %d0
	trap #10
