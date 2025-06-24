// common.c
#include <unistd.h>
#include <string.h>
#include "common.h"
#include <sys/socket.h>  


int send_all(int sockfd, const void *buf, size_t len) {
    const char *ptr = buf;
    while (len > 0) {
        ssize_t sent = send(sockfd, ptr, len, 0);
        if (sent <= 0) return -1;
        ptr += sent;
        len -= sent;
    }
    return 0;
}

int recv_all(int sockfd, void *buf, size_t len) {
    char *ptr = buf;
    while (len > 0) {
        ssize_t recvd = recv(sockfd, ptr, len, 0);
        if (recvd <= 0) return -1;
        ptr += recvd;
        len -= recvd;
    }
    return 0;
}
