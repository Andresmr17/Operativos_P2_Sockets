CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lsodium

OBJS = main.o cliente.o ../encrypt.o ../common.o

all: cliente

cliente: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o cliente
