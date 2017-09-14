.text

.global _start
_start:
	move.l #1, %d0
	movec %d0, %dfc
	movec %d0, %sfc
	
	cinva %bc
	move.l #0x00008000, %d0
	movec %d0, %cacr
	
	lea forever, %a0
	move.l %a0, (%sp)
	bra main
forever:
	bra forever
