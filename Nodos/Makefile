CC = gcc
CFLAGS = -Wall -O2 -lsodium

all: nodo

nodo: main.c nodo.c encrypt.c
	$(CC) main.c nodo.c encrypt.c -o nodo $(CFLAGS)

clean:
	rm -f nodo
