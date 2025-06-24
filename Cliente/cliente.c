// cliente.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../encrypt.h"
#include "../common.h"

#define SERVER_PORT 9000
#define CHUNKS 3

void run_client(const char *filename, const unsigned char *bobsk, const unsigned char *alicepk) {
    printf("[Cliente] Abriendo archivo: %s\n", filename);
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("No se pudo abrir el archivo");
        exit(1);
    }

    // Obtener tamaño del archivo
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek");
        exit(1);
    }

    long file_size = ftell(file);
    if (file_size <= 0) {
        fprintf(stderr, "Archivo vacío o error con ftell (file_size=%ld)\n", file_size);
        exit(1);
    }
    rewind(file);

    printf("[Cliente] Tamaño del archivo: %ld bytes\n", file_size);

    unsigned char *buffer = malloc(file_size);
    if (!buffer) {
        perror("malloc");
        exit(1);
    }

    if (fread(buffer, 1, file_size, file) != file_size) {
        fprintf(stderr, "Error leyendo el archivo\n");
        exit(1);
    }
    fclose(file);

    // Conexión al servidor
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

    size_t len = (size_t)file_size;
    size_t base_len = len / CHUNKS;
    size_t offset = 0;

    printf("DEBUG - valor inicial offset: %zu\n", offset);

    for (int i = 0; i < CHUNKS; i++) {
        size_t chunk_len = (i == CHUNKS - 1) ? len - offset : base_len;
        size_t original_chunk_len = chunk_len;  // 🔧 variable auxiliar

        printf("[Cliente] Fragmento %d: offset=%zu, len=%zu\n", i, offset, chunk_len);
        printf("DEBUG - iteración %d: offset antes=%zu\n", i, offset);

        unsigned char shared_secret[32], symmetric_key[32];
        generate_shared_secret(shared_secret, bobsk, alicepk);
        derive_symmetric_key(symmetric_key, shared_secret);

        unsigned char iv[12], tag[16];
        unsigned char *ciphertext = malloc(chunk_len + 64);
        int ciphertext_len;

        encrypt_message(symmetric_key, buffer + offset, chunk_len, iv, tag, ciphertext, &ciphertext_len);

        send_all(sock, &ciphertext_len, sizeof(int));
        send_all(sock, &chunk_len, sizeof(int));  // puedes enviar chunk_len o original_chunk_len aquí
        send_all(sock, iv, 12);
        send_all(sock, tag, 16);
        send_all(sock, ciphertext, ciphertext_len);

        free(ciphertext);

        offset += original_chunk_len;  // 🔧 usar auxiliar que no se alteró
        printf("DEBUG - iteración %d: offset después=%zu\n", i, offset);
    }


    free(buffer);
    printf("[Cliente] Fragmentos cifrados enviados.\n");
    close(sock);
}
