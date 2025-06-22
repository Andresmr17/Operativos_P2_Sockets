// encrypt.h
#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <sodium.h>

void generate_shared_secret(unsigned char *shared_secret,
                            const unsigned char *sk, const unsigned char *pk);

void derive_symmetric_key(unsigned char *symmetric_key,
                          const unsigned char *shared_secret);

int encrypt_message(const unsigned char *key, const unsigned char *plaintext,
                    int plaintext_len, unsigned char *iv, unsigned char *tag,
                    unsigned char *ciphertext, int *ciphertext_len);

int decrypt_message(const unsigned char *key, const unsigned char *ciphertext,
                    int ciphertext_len, const unsigned char *iv, const unsigned char *tag,
                    unsigned char *plaintext, int *plaintext_len);

#endif
