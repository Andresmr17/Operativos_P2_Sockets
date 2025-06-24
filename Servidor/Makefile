CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lsodium -L../Driver-Biblioteca -luart

OBJS = main.o servidor.o ../common.o uart_control.o

all: servidor

servidor: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o servidor