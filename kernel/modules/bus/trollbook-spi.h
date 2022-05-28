#ifndef TROLLBOOK_SPI_H_
#define TROLLBOOK_SPI_H_

#include <stdint.h>
#include <bus/spi.h>

SpiContext *trollbook_spi_new_context(int slave, uint16_t bauddiv);

#endif
