CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lsodium -L../Driver-Biblioteca -luart

all: servidor

servidor: main.c servidor.c encrypt.c
	$(CC) main.c servidor.c encrypt.c -o servidor $(CFLAGS) $(LDFLAGS)

clean:
	rm -f servidor
