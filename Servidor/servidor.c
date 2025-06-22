// servidor.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "encrypt.h"
#include "common.h"
#include "uart_control.h"

#define SERVER_PORT 9000
#define NODES 3
#define MAX_CIPHER 65536
#define MAX_WORD 128

void run_server(const unsigned char *alicesk, const unsigned char *bobpk) {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(SERVER_PORT), .sin_addr.s_addr = INADDR_ANY };
    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 1);

    printf("[Servidor] Esperando conexión del cliente...\n");
    int client_sock = accept(server_sock, NULL, NULL);
    printf("[Servidor] Cliente conectado.\n");

    // Recibir fragmentos
    int ciphertext_len[NODES], chunk_len[NODES];
    unsigned char ciphertexts[NODES][MAX_CIPHER];
    unsigned char ivs[NODES][12];
    unsigned char tags[NODES][16];

    for (int i = 0; i < NODES; i++) {
        recv_all(client_sock, &ciphertext_len[i], sizeof(int));
        recv_all(client_sock, &chunk_len[i], sizeof(int));
        recv_all(client_sock, ivs[i], 12);
        recv_all(client_sock, tags[i], 16);
        recv_all(client_sock, ciphertexts[i], ciphertext_len[i]);
    }
    close(client_sock);

    // Conectarse a los nodos y enviar fragmentos
    int node_ports[NODES] = {9001, 9002, 9003};
    int node_socks[NODES];

    for (int i = 0; i < NODES; i++) {
        struct sockaddr_in node_addr = { .sin_family = AF_INET, .sin_port = htons(node_ports[i]) };
        inet_pton(AF_INET, "127.0.0.1", &node_addr.sin_addr);
        node_socks[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(node_socks[i], (struct sockaddr*)&node_addr, sizeof(node_addr)) < 0) {
            perror("[Servidor] No se pudo conectar al nodo");
            exit(1);
        }

        send_all(node_socks[i], &ciphertext_len[i], sizeof(int));
        send_all(node_socks[i], &chunk_len[i], sizeof(int));
        send_all(node_socks[i], ivs[i], 12);
        send_all(node_socks[i], tags[i], 16);
        send_all(node_socks[i], ciphertexts[i], ciphertext_len[i]);
    }

    // Recibir respuestas
    char palabras[NODES][MAX_WORD];
    int conteos[NODES];
    for (int i = 0; i < NODES; i++) {
        recv_all(node_socks[i], &conteos[i], sizeof(int));
        recv_all(node_socks[i], palabras[i], MAX_WORD);
        close(node_socks[i]);
    }

    // Determinar la palabra más frecuente
    int max = 0;
    char resultado[MAX_WORD];
    for (int i = 0; i < NODES; i++) {
        if (conteos[i] > max) {
            max = conteos[i];
            strcpy(resultado, palabras[i]);
        }
    }

    printf("[Servidor] Palabra más frecuente: '%s' (%d veces)\n", resultado, max);

    uart_control_init();
    uart_control_write_string(resultado);
    uart_control_write_char(' ');
    char str_count[10];
    sprintf(str_count, "%d", max);
    uart_control_write_string(str_count);
    uart_control_signal_done();
    uart_control_close();

    close(server_sock);
}
