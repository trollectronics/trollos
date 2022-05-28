#ifndef NOP_H_
#define NOP_H_

#define nop() do {__asm__ __volatile__ ("nop\n");} while(0)

#endif
