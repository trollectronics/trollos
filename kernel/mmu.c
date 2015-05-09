#include <chipset.h>
#include "mmu.h"
#include "printf.h"
#include "kernel.h"


struct {
	MmuFreeMemTable table[32];
	uint32_t allocated_frames;
	uint32_t total_frames;
} static mem_layout;

/*Primitive heap for the first stages of the kernel*/
struct {
	uint32_t phys_addr;
	void *start;
	void *end;
} static primitive_heap = {
	0x0,
	(void *) (0xFFFFFFFFUL),
	0x0,
};



static void *primitive_heap_alloc_page(uint32_t *physical_address) {
	/*Returns physical address and physical alloc count in pointer parameters*/
	uint32_t table_number = ((uint32_t) primitive_heap.end) / (4096*1024);
	uint32_t descriptor_number = (((uint32_t) primitive_heap.end)/4096) % 1024;
	uint32_t frame_address = primitive_heap.phys_addr;
	MmuRegRootPointer srp;
	MmuDescriptorShort *page_table;
	MmuDescriptorShort *descriptor_table;
	MmuDescriptorShort descriptor = {
		.page = {
			.descriptor_type = MMU_DESCRIPTOR_TYPE_PAGE,
			.write_protected = false,
			.used = false,
			.modified = false,
			.cache_inhibit = false,
			.page_address = frame_address >> 8,
		}
	};
	
	
	mmu_get_srp(&srp);
	page_table = (void *) (srp.table_address << 4);
	
	mem_layout.allocated_frames++;
	
	if(page_table[table_number].table.descriptor_type != MMU_DESCRIPTOR_TYPE_TABLE_SHORT) {
		MmuDescriptorShort table = {
			.table = {
				.descriptor_type = MMU_DESCRIPTOR_TYPE_TABLE_SHORT,
				.write_protected = false,
				.used = false,
				.table_address = (primitive_heap.phys_addr) >> 4,
			}
		};
		page_table[table_number].whole = table.whole;
		/*need to allocate one extra frame for the page table*/
		primitive_heap.phys_addr += MMU_PAGE_SIZE;
		primitive_heap.end += MMU_PAGE_SIZE;
		mem_layout.allocated_frames++;
	}
	descriptor_table = (void *) (page_table[table_number].table.table_address << 4);
	descriptor_table[descriptor_number].whole = descriptor.whole;
	
	if(physical_address)
		*physical_address = primitive_heap.phys_addr;
	primitive_heap.phys_addr += MMU_PAGE_SIZE;
	primitive_heap.end += MMU_PAGE_SIZE;
	
	return primitive_heap.end - MMU_PAGE_SIZE;
}


static void primitive_heap_init() {
	/*This function assumes page table resides in direct-mapped (phys=virt) LLRAM*/
	MmuRegRootPointer srp;
	MmuDescriptorShort *dir;
	MmuDescriptorShort *table;
	uint32_t i, j;
	uint32_t phys;
	uint32_t logic;
	
	mmu_get_srp(&srp);
	//TODO: check page size, table indices
	if(srp.descriptor_type != MMU_DESCRIPTOR_TYPE_TABLE_SHORT)
		panic("non-supported page table");
	dir = (void *) (srp.table_address << 4);
	
	for(i = MMU_DRAM_START/(MMU_PAGE_SIZE*1024); i < 1024; i++) {
		switch(dir[i].table.descriptor_type) {
			case MMU_DESCRIPTOR_TYPE_TABLE_SHORT:
				table = (void *) (dir[i].table.table_address << 4);
				for(j = 0; j < 1024; j++) {
					switch(table[j].table.descriptor_type) {
						case MMU_DESCRIPTOR_TYPE_PAGE:
							//set physical address of heap
							mem_layout.allocated_frames++;
							phys = (table[j].page.page_address << 8) + MMU_PAGE_SIZE;
							if(phys > primitive_heap.phys_addr) {
								primitive_heap.phys_addr = phys;
							}
							break;
						case MMU_DESCRIPTOR_TYPE_INVALID:
							//set logical address of heap
							logic = (i*1024 + j)*MMU_PAGE_SIZE;
							if(logic < (uint32_t) primitive_heap.start) {
								primitive_heap.start = (void *) logic;
							}
							break;
						default:
							panic("non-supported page table");
					}
				}
				break;
			case MMU_DESCRIPTOR_TYPE_INVALID:
				//set logical address of heap
				logic = (i*1024)*MMU_PAGE_SIZE;
				if(logic < (uint32_t) primitive_heap.start)
					primitive_heap.start = (void *) logic;
				break;
			default:
				panic("non-supported page table");
		}
	}
	primitive_heap.end = primitive_heap.start;
}

static void flag_used(uint32_t frame) {
	uint32_t dir_index = frame/(1024*32);
	uint32_t map_index = (frame/32) % 1024;
	uint32_t block_bit = map_index/32;
	uint32_t bit = frame % 32;
	int i;
	
	if(frame < (MMU_DRAM_START/MMU_PAGE_SIZE))
		return;
	if(frame >= ((MMU_DRAM_START/MMU_PAGE_SIZE) + mem_layout.total_frames ))
		return;
	
	mem_layout.table[dir_index].bitmap[map_index] &= ~(0x1 << bit);
	
	for(i = 0; i < 32; i++) {
		if(mem_layout.table[dir_index].bitmap[block_bit*32 + i])
			return;
	}
	mem_layout.table[dir_index].blocks &= ~(0x1 << block_bit);
}

static int32_t find_unused() {
	uint32_t dir_index, block, blocks, map_index, bit, bits;
	
	for(dir_index = 0; dir_index < 32; dir_index++) {
		if(!(mem_layout.table[dir_index].bitmap && mem_layout.table[dir_index].blocks))
			continue;
		
		blocks = mem_layout.table[dir_index].blocks;
		for(block = 0; !(blocks & 0x1); block++, blocks >>= 1);
		
		for(map_index = block*32; map_index < block*32 + 32; map_index++) {
			if(!mem_layout.table[dir_index].bitmap[map_index])
				continue;
			
			bits = mem_layout.table[dir_index].bitmap[map_index];
			for(bit = 0; !(bits & 0x1); bit++, bits >>= 1);
			
			return dir_index*32*1024 + map_index*32 + bit;
		}
			
	}
	return -1;
}

static void flag_unused(uint32_t frame) {
	uint32_t dir_index = frame/(1024*32);
	uint32_t map_index = (frame/32) % 1024;
	uint32_t block_bit = map_index/32;
	uint32_t bit = frame % 32;
	
	if(frame < (MMU_DRAM_START/MMU_PAGE_SIZE))
		return;
	if(frame >= ((MMU_DRAM_START/MMU_PAGE_SIZE) + mem_layout.total_frames ))
		return;
	
	mem_layout.table[dir_index].bitmap[map_index] |= (0x1 << bit);
	mem_layout.table[dir_index].blocks |= (0x1 << block_bit);
}


static void scan_and_flag() {
	/*This function assumes page table resides in direct-mapped (phys=virt) LLRAM*/
	MmuRegRootPointer srp;
	MmuDescriptorShort *dir;
	MmuDescriptorShort *table;
	uint32_t frame;
	uint32_t i, j;
	
	mmu_get_srp(&srp);
	dir = (void *) (srp.table_address << 4);
	
	for(i = 0; i < 1024; i++) {
		switch(dir[i].table.descriptor_type) {
			case MMU_DESCRIPTOR_TYPE_TABLE_SHORT:
				table = (void *) (dir[i].table.table_address << 4);
				for(j = 0; j < 1024; j++) {
					switch(table[j].table.descriptor_type) {
						case MMU_DESCRIPTOR_TYPE_PAGE:
							frame = (table[j].page.page_address)/16;
							//TODO: move frame from sram to dram
							flag_used(frame);
							mem_layout.allocated_frames++;
							break;
						case MMU_DESCRIPTOR_TYPE_INVALID:
							break;
						default:
							panic("non-supported page table");
					}
				}
				break;
			case MMU_DESCRIPTOR_TYPE_INVALID:
				break;
			default:
				panic("non-supported page table");
		}
	}
	
	//TODO: second pass clearing bits in .blocks
}


uint32_t mmu_allocate_frame() {
	int32_t frame;
	if((frame = find_unused()) < 0)
		return NULL;
	
	flag_used(frame);
	mem_layout.allocated_frames++;
	
	return (((uint32_t) frame)*MMU_PAGE_SIZE);
}


void mmu_deallocate_frame(uint32_t address) {
	flag_unused(address/MMU_PAGE_SIZE);
	mem_layout.allocated_frames--;
}


void mmu_init() {
	uint32_t frame;
	uint32_t bitmaps_to_alloc;
	int i, j, k;
	
	mem_layout.total_frames = *CHIPSET_IO(CHIPSET_IO_PORT_GET_RAM_SIZE) / MMU_PAGE_SIZE;
	primitive_heap_init();
	
	bitmaps_to_alloc = (mem_layout.total_frames + 4096 + (4096*8 - 1)) / (4096*8);
	for(i = 0; i < bitmaps_to_alloc; i++) {
		mem_layout.table[i].bitmap = primitive_heap_alloc_page(NULL);
		for(j = 0; j < 1024; j++) {
			uint32_t block_bit = j/32;
			for(k = 0; k < 32; k++) {
				frame = ((i*1024) + j)*32 + k;
				if(frame >= MMU_DRAM_START/MMU_PAGE_SIZE && frame < (MMU_DRAM_START/MMU_PAGE_SIZE + mem_layout.total_frames)) {
					mem_layout.table[i].bitmap[j] |= (1 << k);
					mem_layout.table[i].blocks |= (1 << block_bit);
				}
			}
		}
	}
	
	/*Traverse page table, flag allocated frames as used*/
	mem_layout.allocated_frames = 0;
	scan_and_flag();
	printf("%lu kB of %lu kB RAM used\n", mem_layout.allocated_frames*MMU_PAGE_SIZE, mem_layout.total_frames*MMU_PAGE_SIZE);
}
