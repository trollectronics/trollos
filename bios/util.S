.text

.global memset
memset:
	link %fp, #0
	
	move.l 12(%fp), %d1
	move.l 8(%fp), %a0
	move.l 16(%fp), %d0
	bra 2f
	
1:
	move.b %d1, (%a0)+
	subq.l #1, %d0
2:
	bne 1b
	
	move.l 8(%fp), %d0
	unlk %fp
	rts


.global memcpy
memcpy:
	link %fp, #0
	
	move.l 8(%fp), %a0
	move.l 12(%fp), %a1
	move.l 16(%fp), %d0
	bra 2f
	
1:
	move.b (%a1)+, (%a0)+
	subq.l #1, %d0
2:
	bne 1b
	
	move.l 8(%fp), %d0
	unlk %fp
	rts
