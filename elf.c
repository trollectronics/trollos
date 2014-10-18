#include "elf.h"
#include "boot_term.h"
#include "mem_addr.h"
#include "util.h"


int elf_load(void *ptr, int argc, char **argv) {
	struct ElfHeader *eh = ptr;
	struct ElfSectionHeader *sh;
	struct ElfProgramHeader *ph;
	int i;
	int (*entry)(int argc, char **argv);

	if (eh->e_ident[0] != ELF_MAGIC1 || eh->e_ident[1] != ELF_MAGIC2 || eh->e_ident[2] != ELF_MAGIC3 ||
	    eh->e_ident[3] != ELF_MAGIC4) {
		term_puts("Invalid ELF magic\n", MEM_PAL_ERR);
		return 0;
	}
	
	if (eh->e_ident[4] != ELF_MAGIC_CLASS) {
		term_puts("This ELF is not for 32-bit systems\n", MEM_PAL_ERR);
		return 0;
	}

	if (eh->e_ident[5] != ELF_MAGIC_ENDIAN) {
		term_puts("This ELF is not in the correct byte endian\n", MEM_PAL_ERR);
		return 0;
	}

	if (eh->e_type != ELF_TYPE_EXEC) {
		term_puts("This ELF is not executable\n", MEM_PAL_ERR);
		return 0;
	}

	if (eh->e_machine != ELF_MACHINE_M68K) {
		term_puts("This ELF is not executable by a Motorola 68000 processor\n", MEM_PAL_ERR);
		return 0;
	}

	if (!eh->e_entry) {
		term_puts("This ELF has no entry point. Assuming 0x10000000\n", MEM_PAL_WARN);
		return 0;
	}
	
	sh = ptr + eh->e_shoff;
	ph = ptr + eh->e_phoff;
	
	for (i = 0; i < eh->e_phnum; i++, ph = ((void *) ph) + eh->e_phentsize) {
		if (ph->p_type != 1)
			continue;
		if (ph->p_vaddr < 0x10000000 || ph->p_vaddr + ph->p_memsz >= 0x13000000) {
			term_puts("One or more sections are out of bounds (0x10000000 .. 0x12FFFFFF)\n", MEM_PAL_ERR);
			return 0;
		}
		
		memcpy((void *) ph->p_vaddr, ptr + ph->p_offset, ph->p_filesz);
	}

	for (i = 0; i < eh->e_shnum; i++, sh = ((void *) sh) + eh->e_shentsize) {
		if (!sh->sh_addr)
			/* If we're lot loading it, don't pay attention */
			continue;
		if (sh->sh_addr < 0x10000000 || sh->sh_addr + sh->sh_size >= 0x13000000) {
			term_puts("One or more sections are out of bounds (0x10000000 .. 0x12FFFFFF)\n", MEM_PAL_ERR);
			return 0;
		}

		if (sh->sh_type == 0)
			continue;
		if (sh->sh_type == 8) {	/* No bits. .bss et al */
			memset((void *) sh->sh_addr, 0, sh->sh_size);
			continue;
		}

		memcpy((void *) sh->sh_addr, ptr + sh->sh_offset, sh->sh_size);
	}

	entry = (void *) eh->e_entry;
	term_puts("ELF sections loaded. PREPARE FOR LIFT-OFF!!\n", MEM_PAL_NEUTRAL);
	return entry(argc, argv);
}

