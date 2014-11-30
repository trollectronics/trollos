.text
.org 0x100

.global mmu_set_tc
mmu_set_tc:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove (%a0), %tc
	unlk %fp
	rts

.global mmu_get_tc
mmu_get_tc:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove %tc, (%a0)
	unlk %fp
	rts
