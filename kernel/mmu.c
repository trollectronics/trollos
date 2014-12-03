#include "mmu.h"

void *mmu_allocate_frame(uint32_t virtual_address, MmuKernelSegment segment, uint32_t count) {
	#if 0
	bool write_protect = false;
	uint32_t table_number = virtual_address / (4096*1024);
	uint32_t descriptor_number = (virtual_address/4096) % (1024);
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
			write_protect = true;
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
	if(descriptor_table[descriptor_number].page.descriptor_type == MMU_DESCRIPTOR_TYPE_PAGE) {
		mmu_allocate_frame(virtual_address + 4096, segment, count - 1);
		return (void *)(((descriptor_table[descriptor_number].page.page_address) << 8) & ~0xFFF);
	}
	descriptor_table[descriptor_number].whole = descriptor.whole;
	
	allocated_frames++;
	
	mmu_allocate_frame(virtual_address + 4096, segment, count - 1); //because i'm bad.
	return (void *) (page_address & ~0xFFF);
	#endif
}
