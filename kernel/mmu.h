#ifndef __MMU_H__
#define __MMU_H__
#include <mmu.h>
#include <stdbool.h>

#define MMU_PAGE_SIZE 4096U
#define MMU_PAGE_MASK 0x00000FFFUL
#define MMU_TABLE_MASK 0x003FFFFFUL
#define MMU_DRAM_START (16*1024*1024)

/* Store this inside the frame itself */
typedef struct MmuFreeFrame MmuFreeFrame;
struct MmuFreeFrame {
	MmuFreeFrame *next;
};

void mmu_init();
void *mmu_alloc_frame();
void mmu_free_frame(void *frame);
void *mmu_alloc_at(void *virt, bool supervisor, bool write_protected);
void mmu_free_at(void *virt, bool supervisor);
void mmu_print_status();
int mmu_init_userspace(MmuRegRootPointer *crp);
void mmu_free_userspace(MmuRegRootPointer *crp);
int mmu_clone_userspace(MmuRegRootPointer *from, MmuRegRootPointer *to);

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
void mmu_invalidate();

#endif
