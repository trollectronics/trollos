.text
arne: .asciz "set 0x%X\n"
arne2: .asciz "long 0x%X\n"

.align 2
.global setjmp
setjmp:
#TODO: save flags
	move.l 4(%sp), %d0
	
	move.l %d7, 0(%d0)
	move.l %d6, 4(%d0)
	move.l %d5, 8(%d0)
	move.l %d4, 12(%d0)
	move.l %d3, 16(%d0)
	move.l %d2, 20(%d0)
	move.l %d1, 24(%d0)
	move.l %a6, 28(%d0)
	move.l %a5, 32(%d0)
	move.l %a4, 36(%d0)
	move.l %a3, 40(%d0)
	move.l %a2, 44(%d0)
	move.l %a1, 48(%d0)
	move.l %a0, 52(%d0)
	move.l %sp, 56(%d0)
	move.l (%sp), %a0
	move.l %a0, 60(%d0)
	
	move.l #0, %d0
	rts

.align 2
.global longjmp
longjmp:
	move.l 4(%sp), %a0
	move.l 8(%sp), %d0
	
	bne 1f
	move.l #1, %d0
1:
	move.l 0(%a0), %d7
	move.l 4(%a0), %d6
	move.l 8(%a0), %d5
	move.l 12(%a0), %d4
	move.l 16(%a0), %d3
	move.l 20(%a0), %d2
	move.l 24(%a0), %d1
	move.l 28(%a0), %a6
	move.l 32(%a0), %a5
	move.l 36(%a0), %a4
	move.l 40(%a0), %a3
	move.l 44(%a0), %a2
	move.l 56(%a0), %sp
	move.l 60(%a0), %a1
	move.l %a1, (%sp)
	move.l 48(%a0), %a1
	move.l 52(%a0), %a0
	
	rts
