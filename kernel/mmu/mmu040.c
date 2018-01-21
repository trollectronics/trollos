#include "mmu040.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>
#include <mem_addr.h>
#include <chipset.h>
#include <errno.h>
#include "../mmu.h"
#include "../util/mem.h"
#include "../util/log.h"
#include "mmu040.h"

#define SRP_URP_DESCRIPTOR_BITS 9

#define ROOT_LEVEL_DESCRIPTOR_BITS 7
#define ROOT_LEVEL_DESCRIPTORS (1 << ROOT_LEVEL_DESCRIPTOR_BITS)
#define POINTER_LEVEL_DESCRIPTOR_BITS 7
#define POINTER_LEVEL_DESCRIPTORS (1 << POINTER_LEVEL_DESCRIPTOR_BITS)
#define PAGE_LEVEL_DESCRIPTOR_BITS 6
#define PAGE_LEVEL_DESCRIPTORS (1 << PAGE_LEVEL_DESCRIPTOR_BITS)

#define ROOT_LEVEL_ADDR_FIELD(a) ((a) >> (POINTER_LEVEL_DESCRIPTOR_BITS + 2))
#define ROOT_LEVEL_FIELD_ADDR(f) ((f) << (POINTER_LEVEL_DESCRIPTOR_BITS + 2))
#if PAGE_SIZE == 4096
#define POINTER_LEVEL_ADDR_FIELD(a) ((a) >> (PAGE_LEVEL_DESCRIPTOR_BITS + 1))
#define POINTER_LEVEL_FIELD_ADDR(f) ((f) << (PAGE_LEVEL_DESCRIPTOR_BITS + 1))
#else
#define POINTER_LEVEL_ADDR_FIELD(a) ((a) >> (PAGE_LEVEL_DESCRIPTOR_BITS + 2))
#define POINTER_LEVEL_FIELD_ADDR(f) ((f) << (PAGE_LEVEL_DESCRIPTOR_BITS + 2))
#endif
#define PAGE_LEVEL_ADDR_FIELD(a) ((a) >> (PAGE_OFFSET_BITS))
#define PAGE_LEVEL_FIELD_ADDR(f) ((f) << (PAGE_OFFSET_BITS))

#define PAGE_DESCRIPTORS_8K 32
#define PAGE_DESCRIPTORS_4K 64

#define UDT_IS_RESIDENT(x) ((x) == MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_RESIDENT || (x) == MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_RESIDENT_ALT)
#define PDT_IS_RESIDENT(x) ((x) == MMU040_PAGE_DESCRIPTOR_TYPE_RESIDENT || (x) == MMU040_PAGE_DESCRIPTOR_TYPE_RESIDENT_ALT)

//Replace the base part of an address (a) with a new base (b)
#define REPAGE(a, b) ((((uint32_t) (b)) & ~MMU_PAGE_MASK) + (((uint32_t) (a)) & (MMU_PAGE_MASK)))

#define MAPPER_PAGES 10

struct {
	uint32_t free_frames;
	uint32_t total_frames;
	
	PhysicalAddress free_frame;
} static _mem_layout;


static Mmu040RootTableDescriptor *_root_td = (void *) MEM_MMU_TABLE_AREA;
//static Mmu040PointerTableDescriptor *pointer_td = (void *) (SDRAM_BASE + ROOT_LEVEL_DESCRIPTORS*sizeof(Mmu040RootTableDescriptor));

struct Mapper {
	Mmu040PageTableDescriptor *page_td;
	Mmu040PageTableDescriptor *page_descriptor;
	void *page[MAPPER_PAGES];
	int mapping;
} static _mapper = {
	.page_td = (void *) (MEM_MMU_TABLE_AREA + PAGE_SIZE),
	.page_descriptor = ((Mmu040PageTableDescriptor *) (MEM_MMU_TABLE_AREA + PAGE_SIZE)) + 2,
	//.page = (void *) (MEM_MMU_TABLE_AREA + PAGE_SIZE*2),
};

static void *_mapping_push(uint32_t physical_address) {
	int mapping = _mapper.mapping;
	Mmu040PageTableDescriptor desc = {
		.page = {
			.physical_address = PAGE_LEVEL_ADDR_FIELD(physical_address),
			.supervisor = true,
			.page_descriptor_type = MMU040_PAGE_DESCRIPTOR_TYPE_RESIDENT,
		},
	};
	
	if(mapping >= MAPPER_PAGES)
		return NULL;
	
	_mapper.page_descriptor[mapping] = desc;
	mmu040_invalidate_page(_mapper.page[mapping]);
	_mapper.mapping++;
	return _mapper.page[mapping];
}

static void _mapping_pop() {
	if(_mapper.mapping > 0)
		_mapper.mapping--;
}

static PhysicalAddress _alloc_frame() {
	PhysicalAddress free;
	PhysicalAddress *frame;
	
	if(!_mem_layout.free_frames)
		return (PhysicalAddress) NULL;
	
	_mem_layout.free_frames--;
	free = _mem_layout.free_frame;
	kprintf(LOG_LEVEL_DEBUG, "alloc frame 0x%X\n", free);
	
	frame = _mapping_push(free);
	
	_mem_layout.free_frame = *frame;
	mmu040_zero_4k(frame);
	
	_mapping_pop();
	
	//mmu_invalidate();
	return free;
}

static void _free_frame(PhysicalAddress frame) {
	PhysicalAddress *free;
	free = _mapping_push(frame);
	*free = _mem_layout.free_frame;
	_mem_layout.free_frame = frame;
	_mapping_pop();
}

static PhysicalAddress _build_free_frame_list() {
	//Mmu040RootTableDescriptor *root_table;
	Mmu040PointerTableDescriptor *pointer_table;
	Mmu040PageTableDescriptor *page_table;
	
	uint32_t i, j, k;
	PhysicalAddress ph;
	
	PhysicalAddress free_frame = (PhysicalAddress) MMU_DRAM_START;
	
	for(i = 0; i < ROOT_LEVEL_DESCRIPTORS; i++) {
		if(!UDT_IS_RESIDENT(_root_td[i].table.upper_level_descriptor_type))
			continue;
		
		ph = ROOT_LEVEL_FIELD_ADDR(_root_td[i].table.table_address);
		pointer_table = (void *) REPAGE(ph, _mapping_push(ph));
		kprintf(LOG_LEVEL_DEBUG, "[%u] pointer table 0x%X -> 0x%X (0x%X)\n", i, (uint32_t) pointer_table, mmu040_test_read(pointer_table), ROOT_LEVEL_FIELD_ADDR(_root_td[i].table.table_address));
		
		for(j = 0; j < POINTER_LEVEL_DESCRIPTORS; j++) {
			if(!UDT_IS_RESIDENT(pointer_table[j].table.upper_level_descriptor_type))
				continue;
			
			ph = POINTER_LEVEL_FIELD_ADDR(pointer_table[j].table.table_address);
			page_table = (void *) REPAGE(ph, _mapping_push(ph));
			kprintf(LOG_LEVEL_DEBUG, "    [%u] page table 0x%X -> 0x%X (0x%X)\n", j, (uint32_t) page_table, mmu040_test_read(page_table), POINTER_LEVEL_FIELD_ADDR(pointer_table[j].table.table_address));
			
			
			for(k = 0; k < PAGE_DESCRIPTORS; k++) {
				if(!PDT_IS_RESIDENT(page_table[k].page.page_descriptor_type))
					continue;
				
				ph = PAGE_LEVEL_FIELD_ADDR(page_table[k].page.physical_address);
				kprintf(LOG_LEVEL_DEBUG, "        [%u] page 0x%X\n", k, (uint32_t) ph);
				
				if(ph >= free_frame) {
					free_frame = ph + MMU_PAGE_SIZE;
					_mem_layout.free_frames--;
				}
			}
			
			_mapping_pop();
		}
		
		_mapping_pop();
	}
	
	PhysicalAddress free, *frame;
	for(free = free_frame; free < (MMU_DRAM_START + _mem_layout.total_frames*MMU_PAGE_SIZE); free += MMU_PAGE_SIZE) {
		frame = _mapping_push(free);
		*frame = _mem_layout.free_frame;
		_mem_layout.free_frame = free;
		_mapping_pop();
	}
	
	return _mem_layout.free_frame;
}

static void _get_table_indices(void *virtual_address, uint32_t *root_table_index, uint32_t *pointer_table_index, uint32_t *page_table_index) {
	uint32_t virtual = (uint32_t) virtual_address;
	*root_table_index = (virtual >> (32 - ROOT_LEVEL_DESCRIPTOR_BITS)) & (ROOT_LEVEL_DESCRIPTORS - 1);
	*pointer_table_index = (virtual >> (32 - ROOT_LEVEL_DESCRIPTOR_BITS - POINTER_LEVEL_DESCRIPTOR_BITS)) & (POINTER_LEVEL_DESCRIPTORS - 1);
	*page_table_index = (virtual >> PAGE_OFFSET_BITS) & (PAGE_LEVEL_DESCRIPTORS - 1);
}


void mmu040_init() {
	int i;
	for(i = 0; i < MAPPER_PAGES; i++) {
		_mapper.page[i] = (void *) (MEM_MMU_TABLE_AREA + PAGE_SIZE*2 + PAGE_SIZE*i);
	}
	
	_mem_layout.total_frames = (64UL*1024UL*1024UL) / MMU_PAGE_SIZE;
	_mem_layout.free_frames = _mem_layout.total_frames;
	_mem_layout.free_frame = (PhysicalAddress) NULL;
	
	_build_free_frame_list();
}


int mmu040_init_userspace(MmuUserspaceHandle *userspace) {
	PhysicalAddress directory;
	Mmu040RegRootPointer *urp;
	
	if(!userspace)
		return -EINVAL;
	
	urp = (void *) userspace;
	
	memset(urp, 0, sizeof(Mmu040RegRootPointer));
	//kprintf(LOG_LEVEL_INFO, "Setting up empty userspace\n");
	if(!(directory = _alloc_frame()))
		return -ENOMEM;
	//memset(dir, 0, MMU_PAGE_SIZE);
	
	urp->root_pointer = directory >> SRP_URP_DESCRIPTOR_BITS;
	mmu040_set_urp(urp);
	
	return 0;
}

void mmu040_free_userspace(MmuUserspaceHandle *userspace) {
	kprintf(LOG_LEVEL_ERROR, "mmu040_free_userspace not implemented\n");
	//MmuDescriptorShort *dir;
	//MmuDescriptorShort *page;
	//int i, j;
	
	//if(!crp || crp->descriptor_type != MMU_DESCRIPTOR_TYPE_TABLE_SHORT)
		//return;
	
	//dir = (void *) (crp->table_address << 4);
	//for(i = 0; i < 1024; i++) {
		//if(dir[i].table.descriptor_type != MMU_DESCRIPTOR_TYPE_TABLE_SHORT)
			//continue;
		
		//page = (void *) (dir[i].table.table_address << 4);
		//for(j = 0; j < 1024; j++) {
			//if(page[i].page.descriptor_type != MMU_DESCRIPTOR_TYPE_PAGE)
				//continue;
			
			////TODO: refcount?
			//mmu_free_frame((void *) (page[i].page.page_address << 8));
		//}
	//}
}

int mmu040_clone_userspace(MmuUserspaceHandle *from, MmuUserspaceHandle *to) {
	Mmu040RegRootPointer *urp_from, *urp_to;
	Mmu040RootTableDescriptor *root_table_from, *root_table_to;
	Mmu040PointerTableDescriptor *pointer_table_from, *pointer_table_to;
	Mmu040PageTableDescriptor *page_table_from, *page_table_to;
	PhysicalAddress ph;
	uint32_t i, j, k;
	uint8_t *page_from, *page_to;
	
	if(!(from && to))
		return -EINVAL;
	
	urp_from = (void *) from;
	urp_to = (void *) to;
	
	ph = urp_from->root_pointer << SRP_URP_DESCRIPTOR_BITS;
	root_table_from = (void *) REPAGE(ph, _mapping_push(ph));
	
	ph = urp_to->root_pointer << SRP_URP_DESCRIPTOR_BITS;
	root_table_to = (void *) REPAGE(ph, _mapping_push(ph));
	
	for(i = 0; i < ROOT_LEVEL_DESCRIPTORS; i++) {
		if(!UDT_IS_RESIDENT(root_table_from[i].table.upper_level_descriptor_type)) {
			root_table_to[i].table.upper_level_descriptor_type = MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_INVALID;
			continue;
		}
		
		if(!(ph = _alloc_frame())) {
			_mapping_pop();
			_mapping_pop();
			return -ENOMEM;
		}
		
		pointer_table_to = _mapping_push(ph);
		root_table_to[i].table.table_address = ROOT_LEVEL_ADDR_FIELD(ph);
		root_table_to[i].table.upper_level_descriptor_type = MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_RESIDENT;
		
		ph = ROOT_LEVEL_FIELD_ADDR(root_table_from[i].table.table_address);
		pointer_table_from = (void *) REPAGE(ph, _mapping_push(ph));
		
		for(j = 0; j < POINTER_LEVEL_DESCRIPTORS; j++) {
			if(!UDT_IS_RESIDENT(pointer_table_from[j].table.upper_level_descriptor_type)) {
				pointer_table_to[j].table.upper_level_descriptor_type = MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_INVALID;
				continue;
			}
			
			if(!(ph = _alloc_frame())) {
				_mapping_pop();
				_mapping_pop();
				_mapping_pop();
				_mapping_pop();
				return -ENOMEM;
			}
			
			page_table_to = _mapping_push(ph);
			pointer_table_to[j].table.table_address = POINTER_LEVEL_ADDR_FIELD(ph);
			pointer_table_to[j].table.upper_level_descriptor_type = MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_RESIDENT;
			
			ph = POINTER_LEVEL_FIELD_ADDR(pointer_table_from[j].table.table_address);
			page_table_from = (void *) REPAGE(ph, _mapping_push(ph));
			
			for(k = 0; k < PAGE_DESCRIPTORS; k++) {
				if(!PDT_IS_RESIDENT(page_table_from[k].page.page_descriptor_type)) {
					page_table_to[k].page.page_descriptor_type = MMU040_PAGE_DESCRIPTOR_TYPE_INVALID;
					continue;
				}
				
				if(!(ph = _alloc_frame())) {
					_mapping_pop();
					_mapping_pop();
					_mapping_pop();
					_mapping_pop();
					_mapping_pop();
					_mapping_pop();
					return -ENOMEM;
				}
				
				page_to = _mapping_push(ph);
				page_table_to[k].whole = page_table_from[k].whole;
				page_table_to[k].page.physical_address = PAGE_LEVEL_ADDR_FIELD(ph);
				
				ph = PAGE_LEVEL_FIELD_ADDR(page_table_from[k].page.physical_address);
				page_from = _mapping_push(ph);
				
				memcpy(page_to, page_from, MMU_PAGE_SIZE);
				
				_mapping_pop();
				_mapping_pop();
			}
			
			_mapping_pop();
			_mapping_pop();
		}
		
		_mapping_pop();
		_mapping_pop();
	}
	
	_mapping_pop();
	_mapping_pop();
	
	return 0;
}

int mmu040_switch_userspace(MmuUserspaceHandle *userspace) {
	if(!userspace)
		return -EINVAL;
	
	mmu040_set_urp((void *) userspace);
	
	return 0;
}

PhysicalAddress mmu_alloc_at(void *virt, bool supervisor, bool write_protected) {
	Mmu040RegRootPointer rp;
	uint32_t root_table_index, pointer_table_index, page_table_index;
	Mmu040RootTableDescriptor *root_table;
	Mmu040PointerTableDescriptor *pointer_table;
	Mmu040PageTableDescriptor *page_table;
	PhysicalAddress frame, ph;
	
	if(supervisor)
		mmu040_get_srp(&rp);
	else
		mmu040_get_urp(&rp);
	
	_get_table_indices(virt, &root_table_index, &pointer_table_index, &page_table_index);
	
	ph = rp.root_pointer << SRP_URP_DESCRIPTOR_BITS;
	root_table = (void *) REPAGE(ph, _mapping_push(ph));
	
	if(UDT_IS_RESIDENT(root_table[root_table_index].table.upper_level_descriptor_type)) {
		ph = ROOT_LEVEL_FIELD_ADDR(root_table[root_table_index].table.table_address);
		pointer_table = (void *) REPAGE(ph, _mapping_push(ph));
	} else {
		ph = _alloc_frame();
		pointer_table = _mapping_push(ph);
		
		root_table[root_table_index].table.table_address = ROOT_LEVEL_ADDR_FIELD(ph);
		root_table[root_table_index].table.upper_level_descriptor_type = MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_RESIDENT;
	}
	
	if(UDT_IS_RESIDENT(pointer_table[pointer_table_index].table.upper_level_descriptor_type)) {
		ph = POINTER_LEVEL_FIELD_ADDR(pointer_table[pointer_table_index].table.table_address);
		page_table = (void *) REPAGE(ph, _mapping_push(ph));
	} else {
		ph = _alloc_frame();
		page_table = _mapping_push(ph);
		
		pointer_table[pointer_table_index].table.table_address = POINTER_LEVEL_ADDR_FIELD(ph);
		pointer_table[pointer_table_index].table.upper_level_descriptor_type = MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_RESIDENT;
	}
	
	if(PDT_IS_RESIDENT(page_table[page_table_index].page.page_descriptor_type)) {
		page_table[page_table_index].page.write_protected = write_protected;
		_free_frame(PAGE_LEVEL_FIELD_ADDR(page_table[page_table_index].page.physical_address));
	}
	
	frame = _alloc_frame();
	
	page_table[page_table_index].page.physical_address = PAGE_LEVEL_ADDR_FIELD(frame);
	page_table[page_table_index].page.supervisor = supervisor;
	page_table[page_table_index].page.cache_mode = MMU040_CACHE_MODE_CACHE_COPY_BACK;
	page_table[page_table_index].page.write_protected = write_protected;
	page_table[page_table_index].page.page_descriptor_type = MMU040_PAGE_DESCRIPTOR_TYPE_RESIDENT;
	
	_mapping_pop();
	_mapping_pop();
	_mapping_pop();
	
	return frame;
}

void mmu_free_at(void *virt, bool supervisor) {
	//TODO: implement
	kprintf(LOG_LEVEL_ERROR, "mmu_free_at not implemented\n");
}

void mmu040_fill_frame(PhysicalAddress frame, int offset, void *src, unsigned int size) {
	uint8_t *map;
	uint8_t *src_byte = src;
	int i;
	
	map = _mapping_push(frame);
	
	for(i = 0; i < size; i++) {
		map[i + offset] = src_byte[i];
	}
	
	_mapping_pop();
}

void mmu040_copy_from_userspace(void *dst, void *src, size_t size) {
	PhysicalAddress phys;
	uint8_t *src_addr, *dst_addr = dst;
	
	if(!size)
		return;
	
	phys = mmu040_get_physical(src, false);
	src_addr = (void *) ((uint32_t) _mapping_push(phys) | (phys & MMU_PAGE_MASK));
	
	for(;;) {
		*dst_addr++ = *src_addr;
		src++;
		
		if(!(--size))
			break;
		
		if(!(((uint32_t) src) & MMU_PAGE_MASK)) {
			_mapping_pop();
			phys = mmu040_get_physical(src, false);
			src_addr = (void *) ((uint32_t) _mapping_push(phys) | (phys & MMU_PAGE_MASK));
		} else {
			src_addr++;
		}
	}
	_mapping_pop();
}

void mmu040_copy_to_userspace(void *dst, void *src, size_t size) {
	PhysicalAddress phys;
	uint8_t *src_addr = src, *dst_addr;
	
	if(!size)
		return;
	
	phys = mmu040_get_physical(dst, false);
	dst_addr = (void *) ((uint32_t) _mapping_push(phys) | (phys & MMU_PAGE_MASK));
	
	for(;;) {
		*dst_addr = *src_addr++;
		dst++;
		
		if(!(--size))
			break;
		
		if(!(((uint32_t) dst) & MMU_PAGE_MASK)) {
			_mapping_pop();
			phys = mmu040_get_physical(dst, false);
			dst_addr = (void *) ((uint32_t) _mapping_push(phys) | (phys & MMU_PAGE_MASK));
		} else {
			dst_addr++;
		}
	}
	_mapping_pop();
}

void mmu040_map_current_userspace() {
	Mmu040RegRootPointer srp, urp;
	Mmu040RootTableDescriptor *supervisor_root_table, *user_root_table;
	PhysicalAddress ph;
	uint32_t userspace_descriptors = (ROOT_LEVEL_DESCRIPTORS / 4) * 3;
	uint32_t i;
	
	mmu040_get_srp(&srp);
	mmu040_get_urp(&urp);
	
	ph = srp.root_pointer << SRP_URP_DESCRIPTOR_BITS;
	supervisor_root_table = (void *) REPAGE(ph, _mapping_push(ph));
	
	ph = urp.root_pointer << SRP_URP_DESCRIPTOR_BITS;
	user_root_table = (void *) REPAGE(ph, _mapping_push(ph));
	
	for(i = 0; i < userspace_descriptors; i++) {
		supervisor_root_table[i] = user_root_table[i];
	}
	
	_mapping_pop();
	_mapping_pop();
}

void mmu_print_status() {
	kprintf(LOG_LEVEL_INFO, "%lu kB of %lu kB RAM used\n", (_mem_layout.total_frames - _mem_layout.free_frames)*(MMU_PAGE_SIZE/1024), _mem_layout.total_frames*(MMU_PAGE_SIZE/1024));
}
