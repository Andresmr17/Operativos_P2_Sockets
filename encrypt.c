// encrypt.c
#include <string.h>
#include "encrypt.h"
#include <sodium.h>

void generate_shared_secret(unsigned char *shared_secret,
                            const unsigned char *sk, const unsigned char *pk) {
    crypto_scalarmult(shared_secret, sk, pk);
}

void derive_symmetric_key(unsigned char *symmetric_key,
                          const unsigned char *shared_secret) {
    crypto_generichash(symmetric_key, 32, shared_secret, 32, NULL, 0);
}

int encrypt_message(const unsigned char *key, const unsigned char *plaintext,
                    size_t plaintext_len, unsigned char *iv, unsigned char *tag,
                    unsigned char *ciphertext, int *ciphertext_len) {
    randombytes_buf(iv, 12);

    if (crypto_aead_chacha20poly1305_ietf_encrypt(ciphertext, (unsigned long long *)ciphertext_len,
            plaintext, plaintext_len,
            NULL, 0,
            NULL, iv, key) != 0) {
        return -1;
    }

    memcpy(tag, ciphertext + *ciphertext_len - 16, 16);  // extraer tag
    *ciphertext_len -= 16;
    return 0;
}

int decrypt_message(const unsigned char *key, const unsigned char *ciphertext,
                    int ciphertext_len, const unsigned char *iv, const unsigned char *tag,
                    unsigned char *plaintext, int *plaintext_len) {
    unsigned char buffer[ciphertext_len + 16];
    memcpy(buffer, ciphertext, ciphertext_len);
    memcpy(buffer + ciphertext_len, tag, 16);

    if (crypto_aead_chacha20poly1305_ietf_decrypt(plaintext, (unsigned long long *)plaintext_len,
            NULL, buffer, ciphertext_len + 16,
            NULL, 0,
            iv, key) != 0) {
        return -1;
    }
    return 0;
}
