// common.h
#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>

ssize_t send_all(int sockfd, const void *buffer, size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent = send(sockfd, (char*)buffer + total_sent, length - total_sent, 0);
        if (sent <= 0) return -1;
        total_sent += sent;
    }
    return total_sent;
}

ssize_t recv_all(int sockfd, void *buffer, size_t length) {
    size_t total_recv = 0;
    while (total_recv < length) {
        ssize_t received = recv(sockfd, (char*)buffer + total_recv, length - total_recv, 0);
        if (received <= 0) return -1;
        total_recv += received;
    }
    return total_recv;
}

#endif
