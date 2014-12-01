#include "mem_addr.h"
#include "sd.h"
#include "spi.h"
#include "util.h"
#include "boot_term.h"


void sd_init_clk() {
	struct SpiMem spi_mem;
	
//	memset(MEM_SPI_SEND1, 0xFF, 32);

	/* Send 32 bytes of nonsense */
	SPI_REG_MEM->offset = 0x20;

	spi_slave_setup(SPI_SLAVE_SDCARD, false, false, false, 0);
	spi_start(false, false, true, false);

	spi_wait_done();
}


int sd_init_cmd() {
	int i, j, k;
	
	j = 5;
	MEM_SPI_SEND1[j--] = 0x40;
	MEM_SPI_SEND1[j--] = 0;
	MEM_SPI_SEND1[j--] = 0;
	MEM_SPI_SEND1[j--] = 0;
	MEM_SPI_SEND1[j--] = 0;
	MEM_SPI_SEND1[j] = 0x95;
	SPI_REG_MEM->offset = 0x5;
	spi_slave_setup(SPI_SLAVE_SDCARD, true, false, false, 0);
	spi_start(false, false, true, false);
	while (SPI_REG_STATE->send);

	SPI_REG_MEM->offset = 0x1;
	MEM_SPI_SEND1[1] = 0xFF;
	MEM_SPI_SEND1[0] = 0xFF;
	MEM_SPI_RECV1[1] = 0xFF;
	MEM_SPI_RECV1[0] = 0xFF;
	spi_start(true, true, true, false);
	
	for (i = 0; (SPI_REG_STATE->send || SPI_REG_STATE->recv) && i < 10000000; i++);
	if (SPI_REG_STATE->send) {
		printf("SPI-SD init timed out\n", 10);
		return 0;
	}

	printf("SPI-SD init successful %X %X\n", MEM_SPI_RECV1[1], MEM_SPI_RECV1[0]);
	return 1;
}


int sd_init() {
//	sd_init_clk();
//	sd_init_cmd();
}
