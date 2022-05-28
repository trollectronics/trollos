#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

typedef struct SpiContext SpiContext;
struct SpiContext {
	int (*activate)(SpiContext *ctx);
	int (*deactivate)(SpiContext *ctx);
	int (*send_recv)(SpiContext *ctx, uint8_t *sdata, uint8_t *rdata, size_t size);

	void *context_private;
};

int spi_context_activate(SpiContext *ctx);
int spi_context_deactivate(SpiContext *ctx);
int spi_send_recv(SpiContext *ctx, uint8_t *sdata, uint8_t *rdata, size_t size); //sdata or rdata may be NULL


#endif
