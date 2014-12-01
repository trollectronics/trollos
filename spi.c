#include "spi.h"

void spi_wait_done() {
	while (SPI_REG_STATE->send || SPI_REG_STATE->recv);
}
