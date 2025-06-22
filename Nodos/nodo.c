// nodo.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "encrypt.h"
#include "common.h"

#define MAX_CIPHER 65536
#define MAX_TEXT 65536
#define MAX_WORD 128

void count_most_frequent(const char *text, char *most_freq, int *count) {
    struct Word {
        char word[MAX_WORD];
        int count;
    } words[1024];

    int word_count = 0;
    char current[MAX_WORD];
    int ci = 0;

    for (int i = 0; ; i++) {
        char c = tolower(text[i]);
        if (isalpha(c)) {
            current[ci++] = c;
        } else if (ci > 0) {
            current[ci] = '\0';
            int found = 0;
            for (int j = 0; j < word_count; j++) {
                if (strcmp(words[j].word, current) == 0) {
                    words[j].count++;
                    found = 1;
                    break;
                }
            }
            if (!found && word_count < 1024) {
                strcpy(words[word_count].word, current);
                words[word_count].count = 1;
                word_count++;
            }
            ci = 0;
        }
        if (text[i] == '\0') break;
    }

    int max = 0;
    strcpy(most_freq, "");
    for (int i = 0; i < word_count; i++) {
        if (words[i].count > max) {
            max = words[i].count;
            strcpy(most_freq, words[i].word);
        }
    }
    *count = max;
}

void run_node(int port, const unsigned char *alicesk, const unsigned char *bobpk) {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[Nodo] Error al crear socket");
        exit(1);
    }

    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port), .sin_addr.s_addr = INADDR_ANY };

    int reuse = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); // Evita "Address already in use"

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[Nodo] Error en bind");
        exit(1);
    }

    if (listen(server_sock, 1) < 0) {
        perror("[Nodo] Error en listen");
        exit(1);
    }

    printf("[Nodo %d] Esperando conexión...\n", port);

    int client = accept(server_sock, NULL, NULL);
    if (client < 0) {
        perror("[Nodo] Error en accept");
        exit(1);
    }

    int cipher_len, plain_len;
    unsigned char iv[12], tag[16];
    unsigned char ciphertext[MAX_CIPHER];
    unsigned char plaintext[MAX_TEXT];

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
    char palabra[MAX_WORD];
    int conteo = 0;
    count_most_frequent((char*)plaintext, palabra, &conteo);

    send_all(client, &conteo, sizeof(int));
    send_all(client, palabra, MAX_WORD);

    printf("[Nodo %d] Procesamiento completo. '%s' (%d)\n", port, palabra, conteo);
    close(client);
    close(server_sock);
}
