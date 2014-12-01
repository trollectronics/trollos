.section .start, "x"

.global _start
_start:
	lea forever, %a0
	move.l %a0, -(%sp)
	bra main
forever:
	bra forever
