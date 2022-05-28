.bss
buf: .space 512

.text
init_text: .asciz "I am init\n"

pathname: .asciz "/test.txt"
exename: .asciz "/sbin/testbin"

.align 2
.global _start
_start:
| open
	move.l #4, %d0
	move.l #pathname, %a0
	move.l #0, %a1
	trap #10
	

| read
	move.l %d0, %a0
	move.l #2, %d0
	move.l #buf, %a1
	move.l #512, %a2
	trap #10

| write stdout	
	move.l #3, %d0
	move.l #1, %a0
	move.l #buf, %a1
	move.l #512, %a2
	trap #10
	
| write fname stdout	
	move.l #3, %d0
	move.l #1, %a0
	move.l #pathname, %a1
	move.l #8, %a2
	trap #10

| fork
	move.l #1, %d0
	trap #10

	cmp.l #0, %d0
	beq execve

testtext:
| test text
	move.l #3, %d0
	move.l #1, %a0
	move.l #init_text, %a1
	move.l #10, %a2
	trap #10

	bra testtext

execve:
| execve
	move.l #10, %d0
	move.l #exename, %a0
	trap #10

	#exit
	move.l #5, %a4
	move.l #4, %a3
	move.l #3, %a2
	move.l #2, %a1
	move.l #1, %a0
	move.l #0, %d0
	trap #10
