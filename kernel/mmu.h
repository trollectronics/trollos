#ifndef _MMU_H
#define _MMU_H

#include "mmu/mmu040.h"

typedef uint32_t PhysicalAddress;
#define MMU_PAGE_MASK 0x00000FFFUL
#define MMU_PAGE_SIZE 4096U
#define MMU_DRAM_START (16*1024*1024)

#define mmu_init mmu040_init
#define mmu_print_status mmu040_print_status
#define mmu_init_userspace mmu040_init_userspace
#define mmu_free_userspace mmu040_free_userspace
#define mmu_clone_userspace mmu040_clone_userspace
#define mmu_alloc_at mmu040_alloc_at
#define mmu_invalidate_all mmu040_invalidate_all
#define mmu_set_crp mmu040_set_urp

#endif
