#include "mmu.h"

#define PAGE_TABLE_SIZE 4096

static void scan_page_table_short(MmuDescriptorShort *entry);
static void scan_page_table_long(MmuDescriptorLong *entry);

static void scan_page_table_short(MmuDescriptorShort *entry) {
	MmuDescriptorShort *s;
	MmuDescriptorLong *l;
	int i;
	
	switch(entry->table.descriptor_type) {
		case MMU_DESCRIPTOR_TYPE_TABLE_SHORT:
			s = (void *) (entry->table.table_address << 4);
			for(i = 0; i < (PAGE_TABLE_SIZE/4); i++)
				scan_page_table_short(&s[i]);
			break;
		case MMU_DESCRIPTOR_TYPE_TABLE_LONG:
			l = (void *) (entry->table.table_address << 4);
			for(i = 0; i < (PAGE_TABLE_SIZE/4); i++)
				scan_page_table_long(&l[i]);
			break;
		case MMU_DESCRIPTOR_TYPE_PAGE:
			/*Flag as used*/
			printf("found used page 0x%X\n", (entry->page.page_address << 4));
			return;
		case MMU_DESCRIPTOR_TYPE_INVALID:
			/*Flag as unused*/
			return;
	}
}

static void scan_page_table_long(MmuDescriptorLong *entry) {
	MmuDescriptorShort *s;
	MmuDescriptorLong *l;
	int i;
	
	switch(entry->table.descriptor_type) {
		case MMU_DESCRIPTOR_TYPE_TABLE_SHORT:
			s = (void *) (entry->table.table_address << 4);
			for(i = 0; i < (PAGE_TABLE_SIZE/4); i++)
				scan_page_table_short(&s[i]);
			break;
		case MMU_DESCRIPTOR_TYPE_TABLE_LONG:
			l = (void *) (entry->table.table_address << 4);
			for(i = 0; i < (PAGE_TABLE_SIZE/4); i++)
				scan_page_table_long(&l[i]);
			break;
		case MMU_DESCRIPTOR_TYPE_PAGE:
			/*Flag as used*/
			printf("found used page 0x%X\n", (entry->page.page_address << 4));
			return;
		case MMU_DESCRIPTOR_TYPE_INVALID:
			/*Flag as unused*/
			return;
	}
}


void mmu_init() {
	MmuRegRootPointer srp;
	MmuDescriptorShort root_table_s;
	MmuDescriptorLong root_table_l;
	int i;
	
	/*Traverse page table, build bitmap of allocated frames*/
	mmu_get_srp(&srp);
	switch(srp.descriptor_type) {
		case MMU_DESCRIPTOR_TYPE_TABLE_SHORT:
			root_table_s.table.table_address = srp.table_address;
			root_table_s.table.descriptor_type = srp.descriptor_type;
			scan_page_table_short(&root_table_s);
			break;
		case MMU_DESCRIPTOR_TYPE_TABLE_LONG:
			root_table_l.table.table_address = srp.table_address;
			root_table_l.table.descriptor_type = srp.descriptor_type;
			scan_page_table_long(&root_table_l);
			break;
	}
	
	
}

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
