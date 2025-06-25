// servidor.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../encrypt.h"
#include "../common.h"
#include "uart_control.h"


#define NODOS 3
#define MAX_WORD 30


void agregar_global(Word **global, int *size, int *cap, const char *palabra, int conteo) {
    for (int i = 0; i < *size; i++) {
        if (strcmp((*global)[i].palabra, palabra) == 0) {
            (*global)[i].conteo += conteo;
            return;
        }
    }
    if (*size == *cap) {
        *cap *= 2;
        *global = realloc(*global, *cap * sizeof(Word));
        if (!*global) {
            perror("realloc global");
            exit(1);
        }
    }
    strncpy((*global)[*size].palabra, palabra, MAX_WORD - 1);
    (*global)[*size].palabra[MAX_WORD - 1] = '\0';
    (*global)[*size].conteo = conteo;
    (*size)++;
}

void run_server(const unsigned char *alicesk, const unsigned char *bobpk) {
    int size = 0;
    int cap = 100;

    Word *global = malloc(cap * sizeof(Word));
    if (!global) {
        perror("malloc global");
        exit(1);
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(9000), .sin_addr.s_addr = INADDR_ANY };
    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 1);
    printf("[Servidor] Esperando conexión del cliente...\n");

    int client = accept(server_sock, NULL, NULL);
    printf("[Servidor] Cliente conectado.\n");

    // reenviar los fragmentos a los 3 nodos
    for (int i = 0; i < NODOS; i++) {
        int cipher_len, plain_len;
        unsigned char iv[12], tag[16];
        recv_all(client, &cipher_len, sizeof(int));
        recv_all(client, &plain_len, sizeof(int));
        recv_all(client, iv, 12);
        recv_all(client, tag, 16);
        unsigned char *ciphertext = malloc(cipher_len);
        recv_all(client, ciphertext, cipher_len);

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in node_addr = { .sin_family = AF_INET, .sin_port = htons(9001 + i), .sin_addr.s_addr = inet_addr("127.0.0.1") };
        if (connect(sock, (struct sockaddr*)&node_addr, sizeof(node_addr)) < 0) {
            perror("[Servidor] No se pudo conectar al nodo");
            exit(1);
        }

        send_all(sock, &cipher_len, sizeof(int));
        send_all(sock, &plain_len, sizeof(int));
        send_all(sock, iv, 12);
        send_all(sock, tag, 16);
        send_all(sock, ciphertext, cipher_len);

        free(ciphertext);

        // recibir resultados
        int total;
        recv_all(sock, &total, sizeof(int));
        Word *buffer = malloc(sizeof(Word) * total);
        recv_all(sock, buffer, sizeof(Word) * total);

        printf("[Servidor] Nodo %d envió %d palabras (hasta 20 primeras):\n", i, total);
        for (int j = 0; j < total && j < 20; j++) {
            printf("  %s: %d\n", buffer[j].palabra, buffer[j].conteo);
        }

        for (int j = 0; j < total; j++) {
            agregar_global(&global, &size, &cap, buffer[j].palabra, buffer[j].conteo);
        }

        free(buffer);
        close(sock);
    }

    // buscar la palabra más frecuente

    char resultado[MAX_WORD] = "";
    int max = 0;
    for (int i = 0; i < size; i++) {
        if (global[i].conteo > max) {
            max = global[i].conteo;
            strcpy(resultado, global[i].palabra);
        }
    }

    printf("[Servidor] Palabra más frecuente: '%s' (%d veces)\n", resultado, max);

    uart_control_init();
    uart_control_write_string(resultado);
    uart_control_write_char(' ');
    char count_str[10];
    sprintf(count_str, "%d", max);
    uart_control_write_string(count_str);
    uart_control_signal_done();
    uart_control_close();

    close(client);
    close(server_sock);
}
