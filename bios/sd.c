#include <mem_addr.h>
#include "sd.h"
#include "spi.h"
#include "util.h"
#include "boot_term.h"


void sd_init_clk() {
	struct SpiMem spi_mem;
	
	memset((void *) MEM_SPI_SEND1, 0xFF, 32);

	/* Send 32 bytes of nonsense */
	SPI_REG_MEM->offset = 0x20;

	spi_slave_setup(SPI_SLAVE_SDCARD, false, false, false, 0);
	spi_start(false, false, true, false);

	spi_wait_done();
}


void sd_send_command(int cmd, uint32_t arg) {
	MEM_SPI_SEND1[5] = 0x40 | (cmd & 0x3F);
	MEM_SPI_SEND1[4] = arg >> 24;
	MEM_SPI_SEND1[3] = arg >> 16;
	MEM_SPI_SEND1[2] = arg >> 8;
	MEM_SPI_SEND1[1] = arg;
	MEM_SPI_SEND1[0] = 0x1;
	SPI_REG_MEM->offset = 0x5;
	spi_slave_setup(SPI_SLAVE_SDCARD, true, true, false, 0);
	spi_start(false, false, true, false);
	while (SPI_REG_STATE->send);
	return;
}


bool sd_wait_timeout() {
	int i;

	for (i = 0; i < 100000 && (SPI_REG_STATE->wait_not_00 || SPI_REG_STATE->wait_not_ff); i++);
	if (i == 100000) {
		SPI_REG_STATE->send = 0, SPI_REG_STATE->recv = 0;
		return false;
	}
	while (SPI_REG_STATE->send || SPI_REG_STATE->recv);
	return true;
}


/* Returns offset to start of reply */
int sd_wait_reply(int reply_length) {
	memset((void *) MEM_SPI_SEND1, 0xFF, reply_length);
	SPI_REG_MEM->offset = reply_length - 1;
	spi_start(true, false, true, true);
	if (!sd_wait_timeout())
		return -1;
	return reply_length - 1;
}


bool sd_enter_ready() {
	int off;
	/* TODO: Send CMD8, required for SDHC */
	sd_send_command(55, 0);
	if (sd_wait_reply(1) < 0) {
		printf("ACMD init timeout\n");
		return false;
	}
	sd_send_command(41, 0);
	off = sd_wait_reply(1);

	if (off < 0) {
		printf("ACMD41 timeout\n");
		return false;
	}
	if (MEM_SPI_RECV1[off] & 0x4)	/* Illegal command.. */
		return false;
	return true;
}


uint32_t sd_get_size() {
	int off;
	uint32_t sect_size, size_cnt, size_mult, size;

	sd_send_command(9, 0);
	if (sd_wait_reply(1) < 0)
		return 0;
	if ((off = sd_wait_reply(19)) < 0)
		return 0;
	/* TODO: Check 	for SDHC, this assumes SDv1 */
	sect_size = 1 << (MEM_SPI_RECV1[off - 6] & 0xF);
	printf("Sector size is %u octets\n", sect_size);
	size_cnt = (MEM_SPI_RECV1[off - 7] & 0x3) << 10;
	size_cnt |= (MEM_SPI_RECV1[off - 8] << 2);
	size_cnt |= (MEM_SPI_RECV1[off - 9] >> 6);
	size_mult = (MEM_SPI_RECV1[off - 10] & 0x3) << 1;
	size_mult |= (MEM_SPI_RECV1[off - 11] >> 7);
	size = (size_cnt + 1) * (1 << (size_mult + 2)) * sect_size / 512;
	printf("Detected SD card of size %u kB (%i %i)\n", size / 2, size_cnt, size_mult);
	return size;
}


int sd_init_cmd() {
	uint32_t count;
	int i, j, k;
	for (k = 0; k < 7; k++) {
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
		
		count = boot_term_get_vsync();
		for (i = 0; (SPI_REG_STATE->send || SPI_REG_STATE->recv) && i < 100000; i++);
		
		if (!SPI_REG_STATE->send && !SPI_REG_STATE->recv) {
			return 1;
		}
	}

	printf("SPI-SD init timed out\n", 10);
	spi_start(false, false, false, false);
	return 0;
}


int sd_init() {
	sd_init_clk();
	if (!sd_init_cmd()) {
		printf("Init failed\n");
		return 0;
	}
	if (!sd_enter_ready()) {
		printf("ACMD41 failed\n");
		return 0;
	}

	if (!sd_get_size()) {
		printf("sd_get_size failed\n");
		return 0;
	}
	printf("SD Init succeeded\n");
	return 1;
}
