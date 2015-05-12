#include <limits.h>
#include <mem_addr.h>
#include <bios_info.h>
#include "elf.h"
#include "mmu.h"
#include "mem.h"
#include "log.h"

static void alloc(void *virt, uint32_t count, bool write_protect) {
	uint32_t i;
	uint32_t base = ((uint32_t) virt) & ~MMU_PAGE_MASK;
	
	for(i = 0; i < count; i++) {
		mmu_alloc((void *) (base + i*MMU_PAGE_SIZE), false, write_protect);
	}
}

int (*(elf_load(void *elf)))(int argc, char **argv) {
	struct ElfHeader *header = elf;
	struct ElfSectionHeader *section_header;
	int i;
	int (*entry)(int argc, char **argv);
	unsigned int count;
	bool write_protect;

	if (header->ident[0] != ELF_MAGIC1 || header->ident[1] != ELF_MAGIC2 || header->ident[2] != ELF_MAGIC3 ||
	    header->ident[3] != ELF_MAGIC4) {
		kprintf(LOG_LEVEL_ERROR, "Invalid ELF magic\n");
		return NULL;
	}
	
	if (header->ident[4] != ELF_CLASS_32BIT) {
		kprintf(LOG_LEVEL_ERROR, "This ELF is not for 32-bit systems\n");
		return NULL;
	}

	if (header->ident[5] != ELF_ENDIAN_BIG) {
		kprintf(LOG_LEVEL_ERROR, "This ELF is not in the correct byte endian\n");
		return NULL;
	}

	if (header->type != ELF_TYPE_EXEC) {
		kprintf(LOG_LEVEL_ERROR, "This ELF is not executable\n");
		return NULL;
	}

	if (header->machine != ELF_MACHINE_M68K) {
		kprintf(LOG_LEVEL_ERROR, "This ELF is not executable by a Motorola 68000 processor\n");
		return NULL;
	}

	if (!header->entry) {
		kprintf(LOG_LEVEL_ERROR, "This ELF has no entry point.\n");
		return NULL;
	}
	
	section_header = elf + header->section_header_offset;
	
	for (i = 0; i < header->section_header_entry_count; i++, section_header = ((void *) section_header) + header->section_header_entry_size) {
		if (!section_header->address)
			/* If we're lot loading it, don't pay attention */
			continue;

		if (!section_header->type)
			continue;
		
		if (!(section_header->flags & ELF_SECTION_HEADER_FLAG_ALLOC))
			continue;
		
		kprintf(LOG_LEVEL_DEBUG, "ELF: i have section @ 0x%X\n", section_header->address);
		
		write_protect = (section_header->flags & ELF_SECTION_HEADER_FLAG_WRITE) ? false : true;
		count = (section_header->size + (MMU_PAGE_SIZE - 1))/MMU_PAGE_SIZE;
		if (section_header->type == ELF_SECTION_HEADER_TYPE_PROGRAM_NOBITS) {
			/*BSS segment*/
			alloc((void *) section_header->address, count, false);
			memset_user((void *) section_header->address, 0, section_header->size);
			continue;
		}
		
		alloc((void *) section_header->address, count, false);
		memcpy_to_user((void *) section_header->address, elf + section_header->offset, section_header->size);
	}
	
	entry = (void *) header->entry;
	
	alloc((void *) (UINT_MAX - MMU_PAGE_SIZE + 1), 1, false);
	return entry;
}
