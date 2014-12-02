#ifndef __ELF_H__
#define	__ELF_H__

#define	ELF_MAGIC1		0x7F
#define	ELF_MAGIC2		'E'
#define	ELF_MAGIC3		'L'
#define	ELF_MAGIC4		'F'
#define	ELF_MAGIC_CLASS		1	// 32-bit arch
#define	ELF_MAGIC_ENDIAN	2	// Correct endian (big)

#define	ELF_TYPE_EXEC		2
#define	ELF_MACHINE_M68K	4

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2

struct ElfHeader {
	unsigned char		e_ident[16];
	unsigned short		e_type;
	unsigned short		e_machine;
	unsigned int		e_version;
	unsigned int		e_entry;
	unsigned int		e_phoff;
	unsigned int		e_shoff;
	unsigned int		e_flags;
	unsigned short		e_ehsize;
	unsigned short		e_phentsize;
	unsigned short		e_phnum;
	unsigned short		e_shentsize;
	unsigned short		e_shnum;
	unsigned short		e_shstrndx;
};


struct ElfSectionHeader {
	unsigned int		sh_name;
	unsigned int		sh_type;
	unsigned int		sh_flags;
	unsigned int		sh_addr;
	unsigned int		sh_offset;
	unsigned int		sh_size;
	unsigned int		sh_link;
	unsigned int		sh_info;
	unsigned int		sh_addralign;
	unsigned int		sh_entsize;
};


struct ElfProgramHeader {
	unsigned int		p_type;
	unsigned int		p_offset;
	unsigned int		p_vaddr;
	unsigned int		p_paddr;
	unsigned int		p_filesz;
	unsigned int		p_memsz;
	unsigned int		p_flags;
	unsigned int		p_align;
};

int elf_run(void *ptr, int argc, char **argv);
int (*(elf_load(void *ptr)))(int argc, char **argv);

#endif
