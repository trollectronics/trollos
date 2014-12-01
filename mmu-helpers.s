.text

hej:	.long 0x82C0AA00

.global mmu_get_physical
mmu_get_physical:
	link %fp, #-4
	pmove %tc, -4(%fp)
	ori.l #0x80000000, -4(%fp)
	pmove -4(%fp), %tc
	#move.l #0x4E020, %a0
	#ptestr #6, (%a0), #7, %a0
	move.l hej, %a1
	move.l (%a1), %d0
	ost:
	bra ost
	pmove (%a1), %tc
	unlk %fp
	rts

.global mmu_enable_and_jump
mmu_enable_and_jump:
	link %fp, #-4
	
	pmove %tc, -4(%fp)
	ori.l #0x80000000, -4(%fp)
	pmove -4(%fp), %tc
	#move.l 8(%fp), %a0
	
	
	move.l #0x1000048, %a0
	ptestr #5, (%a0), #7, %a0
	arne:
	bra arne
	move.l #0x1000048, %a0
	move.l #0x0, %sp
	jmp (%a0)

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

.global mmu_set_srp
mmu_set_srp:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove (%a0), %srp
	unlk %fp
	rts

.global mmu_get_srp
mmu_get_srp:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove %srp, (%a0)
	unlk %fp
	rts

.global mmu_set_crp
mmu_set_crp:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove (%a0), %crp
	unlk %fp
	rts

.global mmu_get_crp
mmu_get_crp:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove %crp, (%a0)
	unlk %fp
	rts

.global mmu_set_tt0
mmu_set_tt0:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove (%a0), %tt0
	unlk %fp
	rts

.global mmu_get_tt0
mmu_get_tt0:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove %tt0, (%a0)
	unlk %fp
	rts

.global mmu_set_tt1
mmu_set_tt1:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove (%a0), %tt1
	unlk %fp
	rts

.global mmu_get_tt1
mmu_get_tt1:
	link %fp, #0
	mov.l 8(%fp), %a0
	pmove %tt1, (%a0)
	unlk %fp
	rts
