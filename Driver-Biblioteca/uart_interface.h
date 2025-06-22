#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

void uart_init();
void uart_write_char(char c);
void uart_write_string(const char *str);
void uart_signal_done();
void uart_close();

#endif
