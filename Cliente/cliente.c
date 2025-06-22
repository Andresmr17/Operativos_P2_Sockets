// cliente.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "encrypt.h"
#include "common.h"

#define SERVER_PORT 9000
#define CHUNKS 3
#define MAX_SIZE 65536

void run_client(const char *filename, const unsigned char *bobsk, const unsigned char *alicepk) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT)
    };
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error conectando al servidor");
        exit(1);
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("No se pudo abrir el archivo");
        exit(1);
    }

    unsigned char buffer[MAX_SIZE];
    size_t len = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    size_t base_len = len / CHUNKS;
    size_t offset = 0;

    for (int i = 0; i < CHUNKS; i++) {
        size_t chunk_len = (i == CHUNKS - 1) ? len - offset : base_len;

        unsigned char shared_secret[32], symmetric_key[32];
        generate_shared_secret(shared_secret, bobsk, alicepk);
        derive_symmetric_key(symmetric_key, shared_secret);

        unsigned char iv[12], tag[16];
        unsigned char ciphertext[chunk_len + 64];
        int ciphertext_len;

        encrypt_message(symmetric_key, buffer + offset, chunk_len, iv, tag, ciphertext, &ciphertext_len);

        // Enviar tamaños primero
        send_all(sock, &ciphertext_len, sizeof(int));
        send_all(sock, &chunk_len, sizeof(int));

        // Enviar iv, tag y datos cifrados
        send_all(sock, iv, 12);
        send_all(sock, tag, 16);
        send_all(sock, ciphertext, ciphertext_len);

        offset += chunk_len;
    }

    printf("[Cliente] Fragmentos cifrados enviados.\n");
    close(sock);
}
