#ifndef __INCLUDE_MEM_ADDR_H__
#define	__INCLUDE_MEM_ADDR_H__

#include <stdint.h>
#include <stddef.h>

#define	MEM_BOOTROM	((volatile void *) 0x0)
#define	MEM_LLRAM		((volatile void *) 0x80000)
#define	MEM_VGA_RAM		((volatile void *) (MEM_LLRAM))
#define	MEM_CHIPSET_SPACE	((volatile void *) 0x100000)
#define	MEM_SDRAM		((volatile void *) 0x80000000UL)

#define PERIPHERAL_SPI_BASE (MEM_CHIPSET_SPACE + 0x800)
#define PERIPHERAL_UART_BASE (MEM_CHIPSET_SPACE + 0x900)
#define PERIPHERAL_VGA_BASE (MEM_CHIPSET_SPACE + 0xA00)
#define PERIPHERAL_SOUND_BASE (MEM_CHIPSET_SPACE + 0xB00)
#define PERIPHERAL_INTCTRL_BASE (MEM_CHIPSET_SPACE + 0x000)

#define	MEM_PAL_ERR		12
#define	MEM_PAL_SUCCESS		10
#define	MEM_PAL_WARN		14
#define	MEM_PAL_NEUTRAL		15

#endif
