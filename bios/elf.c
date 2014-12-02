#include <limits.h>
#include <mem_addr.h>
#include <bios_info.h>
#include <mmu.h>
#include <elf.h>
#include "boot_term.h"
#include "printf.h"
#include "util.h"

int (*(elf_load(void *elf)))(int argc, char **argv) {
	struct ElfHeader *header = elf;
	struct ElfSectionHeader *section_header;
	int i;
	int (*entry)(int argc, char **argv);
	unsigned int count;
	MmuKernelSegment segment;
	void *p;

	if (header->ident[0] != ELF_MAGIC1 || header->ident[1] != ELF_MAGIC2 || header->ident[2] != ELF_MAGIC3 ||
	    header->ident[3] != ELF_MAGIC4) {
		term_puts("Invalid ELF magic\n", MEM_PAL_ERR);
		return NULL;
	}
	
	if (header->ident[4] != ELF_CLASS_32BIT) {
		term_puts("This ELF is not for 32-bit systems\n", MEM_PAL_ERR);
		return NULL;
	}

	if (header->ident[5] != ELF_ENDIAN_BIG) {
		term_puts("This ELF is not in the correct byte endian\n", MEM_PAL_ERR);
		return NULL;
	}

	if (header->type != ELF_TYPE_EXEC) {
		term_puts("This ELF is not executable\n", MEM_PAL_ERR);
		return NULL;
	}

	if (header->machine != ELF_MACHINE_M68K) {
		term_puts("This ELF is not executable by a Motorola 68000 processor\n", MEM_PAL_ERR);
		return NULL;
	}

	if (!header->entry) {
		term_puts("This ELF has no entry point. Assuming 0x10000000\n", MEM_PAL_WARN);
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
		
		segment = (section_header->flags & ELF_SECTION_HEADER_FLAG_WRITE) ? MMU_KERNEL_SEGMENT_DATA : MMU_KERNEL_SEGMENT_TEXT;
		count = (section_header->size + 4095)/4096;
		if (section_header->type == 8) {	/* No bits. .bss et al */
			p = mmu_allocate_frame(section_header->address, segment, count);
			memset(p + (section_header->address & 0xFFF), 0, section_header->size);
			continue;
		}
		
		p = mmu_allocate_frame(section_header->address, segment, count);
		memcpy(p + (section_header->address & 0xFFF), elf + section_header->offset, section_header->size);
	}
	
	entry = (void *) header->entry;
	
	mmu_allocate_frame(UINT_MAX - 4096 + 1, MMU_KERNEL_SEGMENT_STACK, 1);
	return entry;
}
