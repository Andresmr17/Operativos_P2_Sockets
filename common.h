// common.h
#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

#define MAX_WORD 64

typedef struct {
    char palabra[MAX_WORD];
    int conteo;
} Word;

int send_all(int sockfd, const void *buf, size_t len);
int recv_all(int sockfd, void *buf, size_t len);

#endif
