#ifndef __INCLUDE_MMU_H__
#define __INCLUDE_MMU_H__
#include <stdint.h>
#include <stdbool.h>
#include "mem_addr.h"

typedef enum MmuKernelSegment MmuKernelSegment;
enum MmuKernelSegment {
	MMU_KERNEL_SEGMENT_TEXT,
	MMU_KERNEL_SEGMENT_DATA,
	MMU_KERNEL_SEGMENT_STACK,
};

typedef enum MmuPageSize MmuPageSize;
enum MmuPageSize {
	MMU_PAGE_SIZE_256 = 0x8,
	MMU_PAGE_SIZE_512,
	MMU_PAGE_SIZE_1K,
	MMU_PAGE_SIZE_2K,
	MMU_PAGE_SIZE_4K,
	MMU_PAGE_SIZE_8K,
	MMU_PAGE_SIZE_16K,
	MMU_PAGE_SIZE_32K,
};

typedef struct MmuRegTranslationControl MmuRegTranslationControl;
struct MmuRegTranslationControl {
	uint32_t enable : 1;
	uint32_t : 5;
	uint32_t supervisor_root_pointer : 1;
	uint32_t function_code_lookup : 1;
	uint32_t page_size : 4;
	uint32_t initial_shift : 4;
	uint32_t table_indices_a : 4;
	uint32_t table_indices_b : 4;
	uint32_t table_indices_c : 4;
	uint32_t table_indices_d : 4;
};

typedef struct MmuRegTransparentTranslation MmuRegTransparentTranslation;
struct MmuRegTransparentTranslation {
	uint32_t logical_address_base : 8;
	uint32_t logical_address_mask : 8;
	uint32_t enable : 1;
	uint32_t : 4;
	uint32_t cache_inhibit : 1;
	uint32_t read_write : 1;
	uint32_t read_write_mask : 1;
	uint32_t : 1;
	uint32_t function_code_base : 3;
	uint32_t : 1;
	uint32_t function_code_mask : 3;
};

typedef struct MmuRegStatus MmuRegStatus;
struct MmuRegStatus {
	uint16_t bus_error : 1;
	uint16_t limit_violation : 1;
	uint16_t supervisor_only : 1;
	uint16_t : 1;
	uint16_t write_protected : 1;
	uint16_t invalid : 1;
	uint16_t modified : 1;
	uint16_t : 2;
	uint16_t transparent : 1;
	uint16_t : 3;
	uint16_t number_of_levels : 3;
};

typedef struct MmuRegRootPointer MmuRegRootPointer;
struct MmuRegRootPointer {
	uint64_t lu : 1;
	uint64_t limit : 15;
	uint64_t : 14;
	uint64_t descriptor_type : 2;
	uint64_t table_address : 28;
	uint64_t : 4;
};

typedef struct MmuDescriptorTableShort MmuDescriptorTableShort;
struct MmuDescriptorTableShort {
	uint32_t table_address : 28;
	uint32_t used : 1;
	uint32_t write_protected : 1;
	uint32_t descriptor_type : 2;
};

typedef struct MmuDescriptorTableLong MmuDescriptorTableLong;
struct MmuDescriptorTableLong {
	uint64_t lu : 1;
	uint64_t limit : 15;
	uint64_t : 7;
	uint64_t supervisor_only : 1;
	uint64_t : 4;
	uint64_t used : 1;
	uint64_t write_protected : 1;
	uint64_t descriptor_type : 2;
	uint64_t table_address : 28;
	uint64_t : 4;
};

typedef struct MmuDescriptorEarlyTerminationShort MmuDescriptorEarlyTerminationShort;
struct MmuDescriptorEarlyTerminationShort {
	uint32_t page_address : 24;
	uint32_t : 1;
	uint32_t cache_inhibit : 1;
	uint32_t : 1;
	uint32_t modified : 1;
	uint32_t used : 1;
	uint32_t write_protected : 1;
	uint32_t descriptor_type : 2;
};

typedef struct MmuDescriptorEarlyTerminationLong MmuDescriptorEarlyTerminationLong;
struct MmuDescriptorEarlyTerminationLong {
	uint64_t lu : 1;
	uint64_t limit : 15;
	uint64_t : 7;
	uint64_t supervisor_only : 1;
	uint64_t : 1;
	uint64_t cache_inhibit : 1;
	uint64_t : 1;
	uint64_t modified : 1;
	uint64_t used : 1;
	uint64_t write_protected : 1;
	uint64_t descriptor_type : 2;
	uint64_t page_address : 24;
	uint64_t : 8;
};

typedef struct MmuDescriptorPageShort MmuDescriptorPageShort;
struct MmuDescriptorPageShort {
	uint32_t page_address : 24;
	uint32_t : 1;
	uint32_t cache_inhibit : 1;
	uint32_t : 1;
	uint32_t modified : 1;
	uint32_t used : 1;
	uint32_t write_protected : 1;
	uint32_t descriptor_type : 2;
};

typedef struct MmuDescriptorPageLong MmuDescriptorPageLong;
struct MmuDescriptorPageLong {
	uint64_t : 16;
	uint64_t : 7;
	uint64_t supervisor_only : 1;
	uint64_t : 1;
	uint64_t cache_inhibit : 1;
	uint64_t : 1;
	uint64_t modified : 1;
	uint64_t used : 1;
	uint64_t write_protected : 1;
	uint64_t descriptor_type : 2;
	uint64_t page_address : 24;
	uint64_t : 8;
};

typedef struct MmuDescriptorInvalidShort MmuDescriptorInvalidShort;
struct MmuDescriptorInvalidShort {
	uint32_t : 30;
	uint32_t descriptor_type : 2;
};

typedef struct MmuDescriptorInvalidLong MmuDescriptorInvalidLong;
struct MmuDescriptorInvalidLong {
	uint64_t : 30;
	uint64_t descriptor_type : 2;
	uint64_t : 32;
};

typedef struct MmuDescriptorIndirectShort MmuDescriptorIndirectShort;
struct MmuDescriptorIndirectShort {
	uint32_t descriptor_address : 30;
	uint32_t descriptor_type : 2;
};

typedef struct MmuDescriptorIndirectLong MmuDescriptorIndirectLong;
struct MmuDescriptorIndirectLong {
	uint64_t : 30;
	uint64_t descriptor_type : 2;
	uint64_t descriptor_address : 30;
	uint64_t : 2;
};

typedef union MmuDescriptorShort MmuDescriptorShort;
union MmuDescriptorShort {
	uint32_t whole;
	struct MmuDescriptorInvalidShort invalid;
	struct MmuDescriptorPageShort page;
	struct MmuDescriptorTableShort table;
	struct MmuDescriptorIndirectShort indirect;
	struct MmuDescriptorEarlyTerminationShort early;
};

typedef union MmuDescriptorLong MmuDescriptorLong;
union MmuDescriptorLong {
	uint64_t whole;
	struct MmuDescriptorInvalidLong invalid;
	struct MmuDescriptorPageLong page;
	struct MmuDescriptorTableLong table;
	struct MmuDescriptorIndirectLong indirect;
	struct MmuDescriptorEarlyTerminationLong early;
};

typedef enum MmuDescriptorType MmuDescriptorType;
enum MmuDescriptorType {
	MMU_DESCRIPTOR_TYPE_INVALID,
	MMU_DESCRIPTOR_TYPE_PAGE,
	MMU_DESCRIPTOR_TYPE_TABLE_SHORT,
	MMU_DESCRIPTOR_TYPE_TABLE_LONG,
};

#endif
