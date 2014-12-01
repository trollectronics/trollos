#ifndef __MEM_ADDR_H__
#define	__MEM_ADDR_H__

#include <stdint.h>

#define	MEM_LLRAM		((volatile void *) 0x80000)
#define	MEM_PAL_RAM		((volatile void *) (0x1400 + MEM_LLRAM))
#define	MEM_VGA_RAM		((volatile void *) (0x1800 + MEM_LLRAM))
#define	MEM_CHIPSET_SPACE	((volatile void *) 0x200000)
#define	MEM_SPI_SEND1		((volatile uint8_t *) (0x78000 + MEM_LLRAM))
#define	MEM_SPI_SEND2		((volatile uint8_t *) (0x7A000 + MEM_LLRAM))
#define	MEM_SPI_RECV1		((volatile uint8_t *) (0x7C000 + MEM_LLRAM))
#define	MEM_SPI_RECV2		((volatile uint8_t *) (0x7E000 + MEM_LLRAM))
#define	MEM_SPI_SIZE		(0x2000)

#define	MEM_PAL_ERR		12
#define	MEM_PAL_SUCCESS		10
#define	MEM_PAL_WARN		14
#define	MEM_PAL_NEUTRAL		15

#endif
