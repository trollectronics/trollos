#include <chipset.h>
#include "mmu.h"
#include "mem.h"
#include "printf.h"
#include "kernel.h"

struct {
	uint32_t free_frames;
	uint32_t total_frames;
	
	MmuFreeFrame *free_frame;
} static mem_layout;

static MmuFreeFrame *find_first_free() {
	MmuRegRootPointer srp;
	MmuDescriptorShort *dir;
	MmuDescriptorShort *table;
	uint32_t i, j;
	void *free = NULL, *tmp;
	
	
	mmu_get_srp(&srp);
	dir = (void *) (srp.table_address << 4);
	
	for(i = MMU_DRAM_START/(MMU_PAGE_SIZE*1024); i < 1024; i++) {
		if(dir[i].table.descriptor_type != MMU_DESCRIPTOR_TYPE_TABLE_SHORT)
			continue;
		
		table = (void *) (dir[i].table.table_address << 4);
		for(j = 0; j < 1024; j++) {
			if(table[i].page.descriptor_type != MMU_DESCRIPTOR_TYPE_PAGE)
				continue;
			
			tmp = (void *) (table[i].page.page_address << 8);
			if(tmp > free)
				free = tmp;
		}
	}
	return free;
}

void mmu_init() {
	/*This function assumes page table resides in direct-mapped (phys=virt) LLRAM*/
	MmuRegRootPointer srp;
	MmuDescriptorShort *dir;
	uint32_t i;
	MmuFreeFrame *free;
	
	mmu_get_srp(&srp);
	dir = (void *) (srp.table_address << 4);
	
	for(i = MMU_DRAM_START/(MMU_PAGE_SIZE*1024); i < 0xC0000000/(MMU_PAGE_SIZE*1024); i++) {
		dir[i].whole = 0x0;
		dir[i].early.descriptor_type = MMU_DESCRIPTOR_TYPE_PAGE;
		
		dir[i].early.page_address = (i * (MMU_PAGE_SIZE*1024)) >> 8;
	}
	mmu_invalidate();
	
	mem_layout.total_frames = *CHIPSET_IO(CHIPSET_IO_PORT_GET_RAM_SIZE) / MMU_PAGE_SIZE;
	mem_layout.free_frames = 0;
	mem_layout.free_frame = NULL;

	/* Build linked list of free frames */
	for(free = find_first_free(); free < (MmuFreeFrame *) (MMU_DRAM_START + mem_layout.total_frames*MMU_PAGE_SIZE); free += MMU_PAGE_SIZE/sizeof(MmuFreeFrame)) {
		free->next = mem_layout.free_frame;
		mem_layout.free_frame = free;
		mem_layout.free_frames++;
	}
}

static void *alloc_frame() {
	MmuFreeFrame *free;
	
	if(!mem_layout.free_frames)
		return NULL;
	
	mem_layout.free_frames--;
	free = mem_layout.free_frame;
	mem_layout.free_frame = free->next;
	
	memset(free, 0, MMU_PAGE_SIZE);
	mmu_invalidate();
	return free;
}

static void free_frame(void *frame) {
	MmuFreeFrame *free = frame; //Mask?
	free->next = mem_layout.free_frame;
	mem_layout.free_frame = free;
	mem_layout.free_frames++;
	mmu_invalidate();
}

void *mmu_alloc(void *virt, bool supervisor, bool write_protected) {
	MmuRegRootPointer rp;
	MmuDescriptorShort *dir;
	MmuDescriptorShort *page;
	uint32_t table, index;
	void *ret;
	
	if(supervisor)
		mmu_get_srp(&rp);
	else
		mmu_get_crp(&rp);
	
	dir = (void *) (rp.table_address << 4);
	table = ((uint32_t) virt)/(MMU_PAGE_SIZE*1024U);
	
	switch(dir[table].table.descriptor_type) {
		case MMU_DESCRIPTOR_TYPE_PAGE:
			/* Already allocated */
			return NULL;
		case MMU_DESCRIPTOR_TYPE_INVALID:
			printf("Will allocate extra frame for page table\n");
			if(!(page = alloc_frame()))
				panic("Out of memory");
			memset(page, 0, MMU_PAGE_SIZE);
			dir[table].whole = 0x0;
			dir[table].table.descriptor_type = MMU_DESCRIPTOR_TYPE_TABLE_SHORT;
			//dir[table].table.write_protected = write_protected;
			dir[table].table.table_address = (((uint32_t) page) >> 4);
			break;
		case MMU_DESCRIPTOR_TYPE_TABLE_LONG:
			panic("Invalid page table");
		case MMU_DESCRIPTOR_TYPE_TABLE_SHORT:
			page = (void *) (dir[table].table.table_address << 4);
	}
	index = (((uint32_t) virt)/MMU_PAGE_SIZE) % 1024U;
	printf("0x%X is in table %i with index %i\n", virt, table, index);
	if(!(ret = alloc_frame()))
		panic("Out of memory");
	memset(ret, 0, MMU_PAGE_SIZE);
	printf(" - phys @ 0x%X\n", ret);
	printf("page table is @ 0x%X\n", page);
	page[index].whole = 0x0;
	page[index].page.descriptor_type = MMU_DESCRIPTOR_TYPE_PAGE;
	page[index].page.page_address = (((uint32_t) ret) >> 8);
	page[index].page.write_protected = write_protected;
	
	return ret;
}

void *mmu_free(void *virt, bool supervisor) {
	//TODO: implement
}

void mmu_print_status() {
	printf("%lu kB of %lu kB RAM used\n", (mem_layout.total_frames - mem_layout.free_frames)*(MMU_PAGE_SIZE/1024), mem_layout.total_frames*(MMU_PAGE_SIZE/1024));
}
