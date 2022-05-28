#include <errno.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <nop.h>
#include <bus/spi.h>
#include "../../util/mem.h"

typedef struct RegStatus RegStatus;
struct RegStatus {
	uint32_t busy : 1;
	uint32_t : 15;
	uint32_t clockdiv : 16;
};

typedef struct TrollbookSpiDevice TrollbookSpiDevice;
struct TrollbookSpiDevice {
	uint32_t data;
	RegStatus status;
	uint32_t slaveselect;
};

typedef struct TrollbookSpiContext TrollbookSpiContext;
struct TrollbookSpiContext {
	volatile TrollbookSpiDevice *device;

	int slave;
	uint16_t bauddiv;
};

static void select_slave(TrollbookSpiContext *tspi, int slave) {
	//uint32_t count = 0;
	nop();nop();
	while(tspi->device->status.busy) {
		nop();nop();//printf("%u\n", count++);
	}
	nop();nop();
	tspi->device->slaveselect = slave;
	nop();
}

static uint8_t send_recv_byte(TrollbookSpiContext *tspi, uint8_t dat) {
	//uint32_t count = 0;
	uint8_t ret;
	nop();nop();
	while(tspi->device->status.busy) {
		nop();nop();
	}
	nop();nop();
	tspi->device->data = dat;
	nop();nop(); //Nops force pipeline synchronization, needed when reading from IO devices
	while(tspi->device->status.busy) {
		nop();nop();//printf("%u\n", count++);
	}
	nop();nop();
	ret = tspi->device->data;
	nop();nop();
	return ret;
}

static void set_clockdiv(TrollbookSpiContext *tspi) {
	nop();
	tspi->device->status.clockdiv = tspi->bauddiv;
	nop();
}

static uint16_t get_clockdiv(TrollbookSpiContext *tspi) {
	uint16_t ret;
	nop();
	ret = tspi->device->status.clockdiv;
	nop();
	return ret;
}

static int activate(SpiContext *ctx) {
	assert(ctx != NULL);
	assert(ctx->context_private != NULL);

	TrollbookSpiContext *privctx = ctx->context_private;

	set_clockdiv(privctx);
	select_slave(privctx, privctx->slave);

	return 0;
}

static int deactivate(SpiContext *ctx) {
	assert(ctx != NULL);
	assert(ctx->context_private != NULL);

	TrollbookSpiContext *privctx = ctx->context_private;

	select_slave(privctx, 0);

	return 0;
}

static int send_recv(SpiContext *ctx, uint8_t *sdata, uint8_t *rdata, size_t size) {
	assert(ctx != NULL);
	assert(ctx->context_private != NULL);

	TrollbookSpiContext *privctx = ctx->context_private;

	while (size) {
		uint8_t s = 0xFF;
		uint8_t r;

		if (sdata)
			s = *sdata++;

		r = send_recv_byte(privctx, s);

		if (rdata)
			*rdata++ = r;
	}

	return 0;
}

SpiContext *trollbook_spi_new_context(int slave, uint16_t bauddiv) {
	SpiContext *ctx = NULL;
	TrollbookSpiContext *privctx = NULL;

	if (!(ctx = kmalloc(sizeof(SpiContext))))
		goto fail;

	if (!(privctx = kmalloc(sizeof(TrollbookSpiContext))))
		goto fail;

	privctx->bauddiv = bauddiv;
	privctx->slave = slave;

	ctx->activate = activate;
	ctx->deactivate = deactivate;
	ctx->send_recv = send_recv;

	ctx->context_private = privctx;

	return ctx;

fail:
	kfree(privctx);
	kfree(ctx);
	return NULL;
}
