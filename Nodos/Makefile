CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lsodium

OBJS = main.o nodo.o ../common.o ../encrypt.o

all: nodo

nodo: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o nodo

