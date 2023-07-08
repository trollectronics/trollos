#ifndef UART_H_
#define UART_H_

typedef struct UartBackend UartBackend;
struct UartBackend {
	int (*putc)(UartBackend *uart, int c);
	int (*getc)(UartBackend *uart);
	int (*set_baudrate)(UartBackend *uart, int baudrate);
	int (*get_baudrate)(UartBackend *uart);

	void *private;
};

#endif
