CC = gcc
CFLAGS = -Wall -O2 -lsodium

all: servidor

servidor: main.c servidor.c encrypt.c uart_control.c
	$(CC) main.c servidor.c encrypt.c uart_control.c -o servidor $(CFLAGS)

clean:
	rm -f servidor
