// uart_interface.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static int fd = -1;

int uart_open() {
    if (fd < 0) {
        fd = open("/dev/proyecto_arduino_usb", O_WRONLY);
        if (fd < 0) {
            fprintf(stderr, "[uart_interface] Error al abrir /dev/proyecto_arduino_usb: %s\n",
                    strerror(errno));
            return -1;
        }
    }
    return 0;
}

int uart_write_char(char c) {
    if (uart_open() < 0) return -1;
    return write(fd, &c, 1);
}

int uart_write_string(const char *str) {
    if (uart_open() < 0) return -1;
    return write(fd, str, strlen(str));
}

int uart_close() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
    return 0;
}
