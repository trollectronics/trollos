#ifndef __MMU_H__
#define __MMU_H__
#include <mmu.h>


void mmu_init();
void *mmu_get_physical(void *phys);
void mmu_set_tc(MmuRegTranslationControl *tc);
void mmu_get_tc(MmuRegTranslationControl *tc);
void mmu_set_srp(MmuRegRootPointer *srp);
void mmu_get_srp(MmuRegRootPointer *srp);
void mmu_set_crp(MmuRegRootPointer *crp);
void mmu_get_crp(MmuRegRootPointer *crp);
void mmu_set_tt0(MmuRegTransparentTranslation*tt0);
void mmu_get_tt0(MmuRegTransparentTranslation *tt0);
void mmu_set_tt1(MmuRegTransparentTranslation *tt1);
void mmu_get_tt1(MmuRegTransparentTranslation *tt1);

#endif
