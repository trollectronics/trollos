#ifndef __MMU_H__
#define __MMU_H__
#include <mmu.h>

#define MMU_PAGE_SIZE 4096
#define MMU_DRAM_START (16*1024*1024)

typedef struct MmuFreeMemTable MmuFreeMemTable;
struct MmuFreeMemTable {
	/*set bit = free frame*/
	uint32_t *bitmap;
	/*0x0 = whole bitmap full, 0xFFFFFFFF = whole bitmap empty*/
	uint32_t blocks;
};

void mmu_init();
void *mmu_allocate_page();
void mmu_deallocate_page(void *address);
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
