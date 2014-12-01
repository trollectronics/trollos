.text

hej:	.long 0xDEADBEEF

.global mmu_get_physical
mmu_get_physical:
	link %fp, #-4
	pmove %tc, -4(%fp)
	ori.l #0x80000000, -4(%fp)
	pmove -4(%fp), %tc
	mov.l 8(%fp), %a1
	ptestr #6, (%a1), #7, %a0
	andi.l #0x7FFFFFFF, -4(%fp)
	pmove -4(%fp), %tc
	mov.l (%a0), %d0
	andi.l #0xFFFFFF00, %d0
	mov.l 8(%fp), %d1
	andi.l #0xFFF, %d1
	or.l %d1, %d0
	unlk %fp
	rts

.global mmu_enable_and_jump
mmu_enable_and_jump:
	link %fp, #-4
	pmove %tc, -4(%fp)
	ori.l #0x80000000, -4(%fp)
	pmove -4(%fp), %tc
	move.l 8(%fp), %a0
	move.l #0x0, %sp
	move.l 16(%fp), -(%sp)
	move.l 12(%fp), -(%sp)
	move.l #0, -(%sp)
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
