#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <uart.h>
#include <errno.h>

#include "../../util/mem.h"

typedef union TrollbookUartRegStatus TrollbookUartRegStatus;
union TrollbookUartRegStatus {
	uint32_t reg;
	struct {
		uint32_t tx_empty : 1;
		uint32_t rx_full : 1;
		uint32_t rx_active : 1;
		uint32_t : 29;
	};
};

typedef union TrollbookUartRegCtrl TrollbookUartRegCtrl;
union TrollbookUartRegCtrl {
	uint32_t reg;
	struct {
		uint32_t tx_empty_ie : 1;
		uint32_t rx_full_ie : 1;
		uint32_t : 14;
		uint32_t bauddiv : 16;
	};
};

typedef struct TrollbookUartHw TrollbookUartHw;
struct TrollbookUartHw {
	volatile uint32_t data;
	volatile TrollbookUartRegStatus status;
	volatile TrollbookUartRegCtrl ctrl;

};

typedef struct TrollbookUart TrollbookUart;
struct TrollbookUart {
	volatile TrollbookUartHw *hw;
	uint32_t interrupt;
};

int _isr(uint32_t interrupt, void *data) {
	UartBackend *uart = data;
	TrollbookUart *tbuart = uart->private;

	if (tbuart->hw->status.rx_full) {
		tbuart->hw->ctrl.rx_full_ie = false;

		chardev_read_callback(uart);
	}

	if (tbuart->hw->status.tx_empty) {
		tbuart->hw->ctrl.tx_empty_ie = false;
	}

	return 0;
}


int _putc(UartBackend *uart, int c) {
	TrollbookUart *tbuart = uart->private;

	if (tbuart->hw->status.tx_empty) {
		tbuart->hw->data = c;
		return 0;
	} else {

		tbuart->hw->ctrl.tx_empty_ie = true;
		return -EWOULDBLOCK;
	}

	//check txfull
	//send byte if unblocked
	//enable interrupt, block thread if not
	//
	return 0;

};

int _getc(UartBackend *uart) {
	//check rxfull
	//read byte if unblocked
	//enable interrupt, block thread if not
	//
	return 0;
};

int _set_baudrate(UartBackend *uart, int baudrate) {
	return 0;
}

int _get_baudrate(UartBackend *uart) {
	return 0;
}


UartBackend *trollbook_uart_init(void *base_reg, uint32_t interrupt) {
	UartBackend *uart = NULL;
	TrollbookUart *tbuart = NULL;

	if (!(tbuart = kmalloc(sizeof(TrollbookUart))))
		goto fail;

	if (!(uart = kmalloc(sizeof(UartBackend))))
		goto fail;

	tbuart->hw = base_reg;
	tbuart->interrupt = interrupt;

	uart->putc = _putc,
	uart->getc = _getc,
	uart->set_baudrate = _set_baudrate,
	uart->get_baudrate = _get_baudrate,

	uart->private = tbuart;

	int_isr_register(interrupt, _isr, uart);

	return uart;

fail:
	kfree(tbuart);
	kfree(uart);

	return NULL;
}
