#ifndef __SPI_H__
#define	__SPI_H__

#include <stdbool.h>
#include "mem_addr.h"

#define	SPI_REG_MEM	((volatile struct SpiMem *) (MEM_CHIPSET_SPACE + 0x20))
#define	SPI_REG_LINE	((volatile struct SpiLineSelect *) (MEM_CHIPSET_SPACE + 0x24))
#define	SPI_REG_STATE	((volatile struct SpiState *) (MEM_CHIPSET_SPACE + 0x28))
#define	SPI_INT_ACK	(*((volatile int *) (MEM_CHIPSET_SPACE + 0x2C)) = 0)

struct SpiMem {
	uint32_t	offset		: 14;
};

struct SpiLineSelect {
	uint32_t	slave_sel	: 2;
	uint32_t	ss_enable	: 1;
	uint32_t	slow_clk	: 1;
	uint32_t	int_enable	: 1;
	uint32_t	bank_select	: 1;
};

struct SpiState {
	uint32_t	wait_not_ff	: 1;
	uint32_t	send		: 1;
	uint32_t	recv		: 1;
	uint32_t	wait_not_00	: 1;
};

enum SpiSlave {
	SPI_SLAVE_SDCARD,
	SPI_SLAVE_PERFCTRL,
	SPI_SLAVE_UNASSIGNED,
	SPI_SLAVE_EXT,
};

void spi_wait_done();
void spi_slave_setup(enum SpiSlave slave, bool ss_enable, bool fast_clock, bool int_enable, int bank);
void spi_start(bool wait_for_non_ff, bool wait_for_non_00, bool send, bool recieve);

#endif
