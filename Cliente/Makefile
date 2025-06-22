CC = gcc
CFLAGS = -Wall -O2 -lsodium

all: cliente

cliente: main.c cliente.c encrypt.c
	$(CC) main.c cliente.c encrypt.c -o cliente $(CFLAGS)

clean:
	rm -f cliente
