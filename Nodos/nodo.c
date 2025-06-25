// nodo.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../encrypt.h"
#include "../common.h"

#define MAX_TEXT 1048576 
#define MAX_CIPHER 1049600
#define MAX_WORD 64

void agregar_palabra(Word **lista, int *size, int *cap, const char *palabra) {
    for (int i = 0; i < *size; i++) {
        if (strcmp((*lista)[i].palabra, palabra) == 0) {
            (*lista)[i].conteo++;
            return;
        }
    }
    if (*size == *cap) {
        *cap *= 2;
        *lista = realloc(*lista, *cap * sizeof(Word));
        if (!*lista) {
            perror("realloc");
            exit(1);
        }
    }
    strncpy((*lista)[*size].palabra, palabra, MAX_WORD - 1);
    (*lista)[*size].palabra[MAX_WORD - 1] = '\0';
    (*lista)[*size].conteo = 1;
    (*size)++;
}

void procesar_texto(const char *texto, Word **lista, int *total) {
    int cap = 1000;
    *total = 0;
    *lista = malloc(cap * sizeof(Word));
    if (!*lista) {
        perror("malloc");
        exit(1);
    }

    char palabra[MAX_WORD];
    int pi = 0;
    for (int i = 0;; i++) {
        char c = tolower(texto[i]);
        if (isalpha(c)) {
            if (pi < MAX_WORD - 1)
                palabra[pi++] = c;
        } else if (pi > 0 || c == '\0') {
            palabra[pi] = '\0';
            agregar_palabra(lista, total, &cap, palabra);
            pi = 0;
        }
        if (texto[i] == '\0') break;
    }
}

void run_node(int port, const unsigned char *alicesk, const unsigned char *bobpk) {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port), .sin_addr.s_addr = INADDR_ANY };
    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 1);
    printf("[Nodo %d] Esperando conexión...\n", port);

    int client = accept(server_sock, NULL, NULL);

    int cipher_len, plain_len;
    unsigned char iv[12], tag[16];
    unsigned char ciphertext[MAX_CIPHER];
    unsigned char plaintext[MAX_TEXT + 1];

    recv_all(client, &cipher_len, sizeof(int));
    recv_all(client, &plain_len, sizeof(int));
    recv_all(client, iv, 12);
    recv_all(client, tag, 16);
    recv_all(client, ciphertext, cipher_len);

    unsigned char shared_secret[32], symmetric_key[32];
    generate_shared_secret(shared_secret, alicesk, bobpk);
    derive_symmetric_key(symmetric_key, shared_secret);

    if (decrypt_message(symmetric_key, ciphertext, cipher_len, iv, tag, plaintext, &plain_len) != 0) {
        fprintf(stderr, "[Nodo %d] Error al descifrar\n", port);
        close(client);
        return;
    }

    plaintext[plain_len] = '\0';

    Word *lista = NULL;
    int total_palabras = 0;
    procesar_texto((char*)plaintext, &lista, &total_palabras);

    send_all(client, &total_palabras, sizeof(int));
    send_all(client, lista, total_palabras * sizeof(Word));
    printf("[Nodo %d] Enviadas %d palabras al servidor\n", port, total_palabras);

    free(lista);
    close(client);
    close(server_sock);
}
