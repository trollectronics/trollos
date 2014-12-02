#include "spi.h"

void spi_wait_done() {
	while (SPI_REG_STATE->send || SPI_REG_STATE->recv);
}

void spi_slave_setup(enum SpiSlave slave, bool ss_enable, bool fast_clock, bool int_enable, int bank) {
	struct SpiLineSelect line;

	line.slave_sel = slave;
	line.ss_enable = ss_enable;
	line.slow_clk = !fast_clock;
	line.int_enable = int_enable;
	line.bank_select = bank;

	*SPI_REG_LINE = line;
}

void spi_start(bool wait_for_non_ff, bool wait_for_non_00, bool send, bool recieve) {
	struct SpiState reg;

	reg.wait_not_ff = wait_for_non_ff;
	reg.wait_not_00 = wait_for_non_00;
	reg.send = send;
	reg.recv = recieve;

	*SPI_REG_STATE = reg;
}

