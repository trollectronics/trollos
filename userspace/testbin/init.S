.bss

.text
test_text: .asciz "I am userspace\n"

.align 2
.global _start
_start:
	move.l #3, %d0
	move.l #1, %a0
	move.l #test_text, %a1
	move.l #15, %a2
	trap #10
	
	bra _start
	
	#exit
	move.l #5, %a4
	move.l #4, %a3
	move.l #3, %a2
	move.l #2, %a1
	move.l #1, %a0
	move.l #0, %d0
	trap #10
