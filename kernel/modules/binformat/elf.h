#ifndef __ELF_H__
#define __ELF_H__
#include <elf.h>

int (*(elf_load(void *ptr)))(int argc, char **argv);


#endif
