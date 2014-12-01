#if 0
#include "mem_addr.h"
#include "sd.h"
#include "spi.h"
#include "util.h"
#include "boot_term.h"

static volatile struct SpiMem *spi_reg_mem = SPI_REG_MEM;
static volatile struct SpiLineSelect *spi_reg_line = SPI_REG_LINE;
static volatile struct SpiState *spi_reg_state = SPI_REG_STATE;


void sd_init_clock() {
	struct SpiMem spi_mem;
	
	memset(MEM_SPI_SEND1, 0xFF, MEM_SPI_SIZE);

	/* Send 32 bytes of nonsense */
	SPI_REG_MEM->offset = 0x20;

	spi_slave_setup(SPI_SLAVE_SDCARD, false, false, false, 0);
	spi_start(false, false, true, false);

	spi_wait_done();
}

int sd_init() {
	sd_init_clock();
}

#endif
