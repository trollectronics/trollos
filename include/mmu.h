#ifndef __INCLUDE_MMU_H__
#define __INCLUDE_MMU_H__
#include <stdint.h>
#include <stdbool.h>
#include "mem_addr.h"

typedef uint32_t PhysicalAddress;
typedef uint32_t MmuUserspaceHandle;

/* Use 4k pages */
#define PAGE_DESCRIPTORS PAGE_DESCRIPTORS_4K
#define PAGE_OFFSET_BITS 12
#define PAGE_SIZE (1 << PAGE_OFFSET_BITS)

#define MEM_MMU_TABLE_AREA (16UL*1024UL*1024UL)

#define MMU_PAGE_MASK 0x00000FFFUL
#define MMU_PAGE_SIZE 4096U
#define MMU_DRAM_START 0x80000000UL

#endif
