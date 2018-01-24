.bss
buf: .space 512

.text
init_text: .asciz "I am init\n"

pathname: .asciz "/test.txt"

.align 2
.global _start
_start:
	#try invalid trap
	trap #5
	
	addq.l #1, %d1
	ori.l #1, %d1
	andi.l #1, %d1
	or.l %d1, %d2
	
	move.l #1, %d0
	trap #10
	cmp #0, %d0
1:	bne 1b
	
	move.l #0xFFFF, %d0
1:
	subi.l #1, %d0
	bne 1b
	
	trap #4
	
	move.l #3, %d0
	move.l #1, %a0
	move.l #init_text, %a1
	move.l #10, %a2
	trap #10
	
	move.l #4, %d0
	move.l #pathname, %a0
	move.l #0, %a1
	trap #10
	
	add.l #'0', %d0
	move.b %d0, buf
	move.l #3, %d0
	move.l #1, %a0
	move.l #buf, %a1
	move.l #10, %a2
	trap #10
	
|	move.l %d0, %a0
|	move.l #2, %d0
|	move.l #buf, %a1
|	move.l #512, %a2
|	trap #10
|	
|	move.l #3, %d0
|	move.l #1, %a0
|	move.l #buf, %a1
|	move.l #10, %a2
|	trap #10
	
	#exit
	move.l #5, %a4
	move.l #4, %a3
	move.l #3, %a2
	move.l #2, %a1
	move.l #1, %a0
	move.l #0, %d0
	trap #10
