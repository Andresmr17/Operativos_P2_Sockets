// uart_control.c
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "uart_control.h"

static int fd = -1;

void uart_control_init() {
    fd = open("/dev/proyecto_arduino_usb", O_WRONLY);
    if (fd < 0) {
        perror("[uart_control] No se pudo abrir el dispositivo");
    }
}

void uart_control_write_char(char c) {
    if (fd >= 0) {
        write(fd, &c, 1);
        sleep(2);
    }
}

void uart_control_write_string(const char *str) {
    for (int i = 0; str[i]; i++) {
        uart_control_write_char(str[i]);
    }
}

void uart_control_signal_done() {
    if (fd >= 0) {
        const char *end = "END\n";
        write(fd, end, strlen(end));
    }
}

void uart_control_close() {
    if (fd >= 0) {
        close(fd);
    }
}

int uart_control_read() {
    if (fd < 0) {
        fprintf(stderr, "[uart_control] Dispositivo no abierto\n");
        return -1;
    }

    int valor;
    ssize_t leido = read(fd, &valor, sizeof(valor));
    if (leido < 0) {
        perror("[uart_control] Error al leer desde el dispositivo");
        return -1;
    } else if (leido != sizeof(valor)) {
        fprintf(stderr, "[uart_control] Lectura incompleta: %ld bytes\n", leido);
        return -1;
    }

    return valor;
}
