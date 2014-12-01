#include "mem_addr.h"
#include "sd.h"
#include "spi.h"
#include "util.h"
#include "boot_term.h"

static volatile struct SpiMem *spi_reg_mem = SPI_REG_MEM;
static volatile struct SpiLineSelect *spi_reg_line = SPI_REG_LINE;
static volatile struct SpiState *spi_reg_state = SPI_REG_STATE;


void sd_init_clk() {
	struct SpiMem spi_mem;
	struct SpiLineSelect spi_line;
	struct SpiState spi_state;
	
	memset(MEM_SPI_SEND1, 0xFF, MEM_SPI_SIZE);

	/* Send 32 bytes of nonsense */
	spi_mem.offset = 0x20;
	*spi_reg_mem = spi_mem;

	spi_line.slave_sel = SPI_SLAVE_SDCARD;
	spi_line.ss_enable = 0;
	spi_line.slow_clk = 1;
	spi_line.int_enable = 0;
	spi_line.bank_select = 0;
	*spi_reg_line = spi_line;

	spi_state.wait_not_ff = 0;
	spi_state.send = 1;
	spi_state.recv = 0;
	spi_state.wait_not_00 = 0;
	*spi_reg_state = spi_state;
	if (!*spi_reg_state->send)
		term_puts("Reading status reg failed\n", 12);
	spi_wait_done();
}

int sd_init() {
	sd_init_clock();
}
