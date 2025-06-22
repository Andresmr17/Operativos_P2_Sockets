// uart_control.h
#ifndef UART_CONTROL_H
#define UART_CONTROL_H

void uart_control_init();
void uart_control_write_char(char c);
void uart_control_write_string(const char *str);
void uart_control_signal_done();
void uart_control_close();

#endif
