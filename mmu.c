#include <stdint.h>
#include <stdbool.h>
#include "printf.h"
#include "terminal.h"
#include "mmu.h"

/** This is probably not nessecary for the bootloader, since we won'd deallocate frames.
 *   We should just use a simple counter to count sequentially allocated frames!*/
/*bitmap of allocated physical memory frames*/
//#define MEM_PHYS_SIZE (64*1024*1024)
//uint32_t frame_map[MEM_PHYS_SIZE/4096/32];
#define MMU_LOGIAL_START (16*1024*1024)
uint32_t allocated_frames;

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
		.enable = false,
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
	
	mmu_set_srp(&srp);
	mmu_set_tt0(&tt0);
	mmu_set_tt1(&tt1);
	mmu_set_tc(&tc);
}

void *mmu_allocate_frame(uint32_t virtual_address, bool write_protect, MmuKernelSegment segment, uint32_t count) {
	uint32_t table_number = virtual_address / (4096*1024);
	uint32_t descriptor_number = virtual_address % (4096*1024);
	uint32_t page_address = MMU_LOGIAL_START + (4096*allocated_frames);
	MmuDescriptorShort *descriptor_table;
	MmuDescriptorShort descriptor = {
		.page = {
			.descriptor_type = true,
			.write_protected = write_protect,
			.used = false,
			.modified = false,
			.cache_inhibit = false,
			.page_address = page_address >> 8,
		}
	};
	MmuDescriptorShort *segment_table;
	
	if(!count)
		return NULL;
	
	switch(segment) {
		case MMU_KERNEL_SEGMENT_TEXT:
			segment_table = supervisor.text;
			break;
		case MMU_KERNEL_SEGMENT_DATA:
			segment_table = supervisor.data;
			break;
		case MMU_KERNEL_SEGMENT_STACK:
			segment_table = supervisor.stack;
			break;
		default:
			return NULL;
	}
	
	if(supervisor.page_table[table_number].table.descriptor_type != MMU_DESCRIPTOR_TYPE_TABLE_SHORT) {
		MmuDescriptorShort table = {
			.table = {
				.descriptor_type = MMU_DESCRIPTOR_TYPE_TABLE_SHORT,
				.write_protected = false,
				.used = false,
				.table_address = ((uint32_t) segment_table) >> 4,
			}
		};
		supervisor.page_table[table_number].whole = table.whole;
	}
	descriptor_table = (void *) (supervisor.page_table[table_number].table.table_address << 4);
	descriptor_table[descriptor_number].whole = descriptor.whole;
	
	allocated_frames++;
	
	mmu_allocate_frame(virtual_address + 4096, write_protect, segment, count - 1); //because i'm bad.
	return (void *) page_address;
}

void mmu_bus_error() {
	terminal_set_bg(TERMINAL_COLOR_RED);
	terminal_set_fg(TERMINAL_COLOR_WHITE);
	terminal_puts("PANIC: bus error");
	
	for(;;);
}
