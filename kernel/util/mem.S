.align 4
.global memset_user
memset_user:
	link %fp, #0
	move.l 8(%fp), %a0
	move.l 12(%fp), %d0
	move.l 16(%fp), %d1
	move.l #0x2, %a1
	movec %a1, %dfc
	
	cmp.l #0, %d1
	beq 2f
1:
	moves.b %d0, (%a0)+
	subq.l #1, %d1
	bne 1b
2:
	unlk %fp
	rts

