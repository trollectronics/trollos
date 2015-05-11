.text

.global _start
_start:
	move.l #1, %d0
	movec %d0, %dfc
	movec %d0, %sfc
	
	lea forever, %a0
	move.l %a0, (%sp)
	bra main
forever:
	bra forever
