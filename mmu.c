#include <stdint.h>
#include <stdbool.h>
#include "printf.h"
#include "mmu.h"

static MmuRegRootPointer supervisor_root_pointer;


/** This is probably not nessecary for the bootloader, since we won'd deallocate frames.
 *   We should just use a simple counter to count sequentially allocated frames!*/
/*bitmap of allocated physical memory frames*/
#define MEM_PHYS_SIZE 67108864
uint32_t frame_map[MEM_PHYS_SIZE/4096/32];

struct {
	MmuDescriptorShort page_table[1024];
	MmuDescriptorShort text[1024];
	MmuDescriptorShort data[1024];
	MmuDescriptorShort stack[1024];
} supervisor;

void mmu_init() {
	MmuRegTranslationControl tc = {
		.page_size = MMU_PAGE_SIZE_4K,
		.initial_shift = 0,
		.table_indices_a = 10, /*1024 entries * 4 byte/entry = 4k*/
		.table_indices_b = 10,
		.supervisor_root_pointer = true,
		.enable = true,
	};
	MmuRegTransparentTranslation tt0 = {
		.function_code_mask = 0x3,
		.function_code_base = 0x4,
		.read_write_mask = 0x1,
		.read_write = 0x0,
		.cache_inhibit = true,
		.enable = true,
		.logical_address_mask = 0x0,
		.logical_address_base = 0x0,
	};
	MmuRegTransparentTranslation tt1 = {
		.enable = false,
	};
	MmuRegRootPointer srp = {
		.table_address = ((uint32_t) supervisor.page_table) >> 4,
		.descriptor_type = MMU_DESCRIPTOR_TYPE_TABLE_SHORT,
		.limit = 0x0,
		.lu = false,
	};
	MmuDescriptorShort table = {
		.table = {
			.descriptor_type = MMU_DESCRIPTOR_TYPE_TABLE_SHORT,
			.write_protected = true,
			.used = false,
			.table_address = ((uint32_t) supervisor.text) >> 4,
		}
	};
	/*Kernel .text starts at 16MB*/
	supervisor.page_table[3].whole = table.whole;
	
	table.table.write_protected = false;
	table.table.table_address = ((uint32_t) supervisor.data) >> 4;
	supervisor.page_table[3 + 1].whole = table.whole;
	
	table.table.table_address = ((uint32_t) supervisor.stack) >> 4;
	supervisor.page_table[1023].whole = table.whole;
	
	//TODO: set up supervisor root pointer
}

void mmu_bus_error() {
	
}
