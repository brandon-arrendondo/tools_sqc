/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: crypto_operations_unsafe.c
 *
 * This case demonstrates violations where cryptographic functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Mock cryptographic structures */
typedef struct {
    unsigned char *key;
    size_t key_length;
    int algorithm;
} CryptoKey;

typedef struct {
    void *context;
    CryptoKey *key;
    int mode;
} CryptoContext;

/* NON-COMPLIANT: No validation of key parameters */
CryptoKey *create_key(const unsigned char *key_data, size_t key_length, int algorithm) {
    CryptoKey *key = malloc(sizeof(CryptoKey));
    /* No validation of key_data or key_length */
    key->key = malloc(key_length);  /* key_length could be 0 or excessive */
    memcpy(key->key, key_data, key_length);  /* key_data could be NULL */
    key->key_length = key_length;
    key->algorithm = algorithm;  /* algorithm not validated */
    return key;
}

/* NON-COMPLIANT: No validation of encryption parameters */
size_t encrypt_data(CryptoContext *ctx, const unsigned char *plaintext, size_t plaintext_len,
                   unsigned char *ciphertext, size_t ciphertext_buffer_size) {
    /* No validation of any parameters */
    printf("Encrypting %zu bytes with algorithm %d\n",
           plaintext_len, ctx->key->algorithm);  /* ctx could be NULL */

    /* Mock encryption without validation */
    size_t encrypted_size = plaintext_len + 16;  /* Adding padding simulation */

    if (encrypted_size > ciphertext_buffer_size) {  /* No check if ciphertext is NULL */
        encrypted_size = ciphertext_buffer_size;
    }

    memcpy(ciphertext, plaintext, encrypted_size);  /* Both could be NULL */
    return encrypted_size;
}

/* NON-COMPLIANT: No validation of decryption parameters */
size_t decrypt_data(CryptoContext *ctx, const unsigned char *ciphertext, size_t ciphertext_len,
                   unsigned char *plaintext, size_t plaintext_buffer_size) {
    /* No validation of any parameters */
    printf("Decrypting %zu bytes\n", ciphertext_len);

    /* Mock decryption without validation */
    size_t decrypted_size = ciphertext_len - 16;  /* Removing padding simulation */

    memcpy(plaintext, ciphertext, decrypted_size);  /* Both could be NULL */
    return decrypted_size;
}

/* NON-COMPLIANT: No validation of hash parameters */
void compute_hash(const unsigned char *data, size_t data_len, unsigned char *hash_output, int hash_algorithm) {
    /* No validation of data or hash_output */
    printf("Computing hash of %zu bytes using algorithm %d\n", data_len, hash_algorithm);

    /* Mock hash computation */
    for (size_t i = 0; i < 32; i++) {  /* Assuming SHA-256 output size */
        hash_output[i] = (unsigned char)(data[i % data_len] ^ i);  /* data could be NULL */
    }
}

/* NON-COMPLIANT: No validation of signature parameters */
size_t sign_data(CryptoKey *private_key, const unsigned char *data, size_t data_len,
                unsigned char *signature, size_t signature_buffer_size) {
    /* No validation of any parameters */
    printf("Signing %zu bytes of data\n", data_len);

    /* Mock signature generation */
    size_t signature_size = private_key->key_length;  /* private_key could be NULL */

    if (signature_size > signature_buffer_size) {  /* No check if signature is NULL */
        signature_size = signature_buffer_size;
    }

    memcpy(signature, data, signature_size);  /* Both could be NULL */
    return signature_size;
}

/* NON-COMPLIANT: No validation of verification parameters */
int verify_signature(CryptoKey *public_key, const unsigned char *data, size_t data_len,
                    const unsigned char *signature, size_t signature_len) {
    /* No validation of any parameters */
    printf("Verifying signature of %zu bytes\n", data_len);

    /* Mock signature verification */
    return memcmp(data, signature, signature_len) == 0;  /* Both could be NULL */
}

/* NON-COMPLIANT: No validation of random generation parameters */
void generate_random_bytes(unsigned char *buffer, size_t buffer_size, int entropy_source) {
    /* No validation of buffer or entropy_source */
    printf("Generating %zu random bytes from source %d\n", buffer_size, entropy_source);

    /* Mock random generation */
    for (size_t i = 0; i < buffer_size; i++) {
        buffer[i] = (unsigned char)(rand() % 256);  /* buffer could be NULL */
    }
}

/* NON-COMPLIANT: No validation of key derivation parameters */
void derive_key(const unsigned char *password, size_t password_len, const unsigned char *salt, size_t salt_len,
               int iterations, unsigned char *derived_key, size_t key_len) {
    /* No validation of any parameters */
    printf("Deriving key from password of length %zu\n", password_len);

    /* Mock key derivation */
    for (size_t i = 0; i < key_len; i++) {
        derived_key[i] = password[i % password_len] ^ salt[i % salt_len];  /* Could dereference NULL */
    }
}

int main(void) {
    CryptoKey *null_key = NULL;
    CryptoContext *null_ctx = NULL;
    unsigned char *null_data = NULL;

    /* Examples of dangerous crypto operations */
    // create_key(null_data, 0, -1);  /* NULL data and invalid algorithm */
    // encrypt_data(null_ctx, null_data, 100, null_data, 0);  /* NULL parameters */
    // decrypt_data(null_ctx, null_data, 100, null_data, 0);  /* NULL parameters */
    // compute_hash(null_data, 100, null_data, -1);  /* NULL parameters */
    // sign_data(null_key, null_data, 100, null_data, 0);  /* NULL parameters */
    // verify_signature(null_key, null_data, 100, null_data, 0);  /* NULL parameters */
    // generate_random_bytes(null_data, 100, -1);  /* NULL buffer */
    // derive_key(null_data, 0, null_data, 0, -1, null_data, 32);  /* NULL parameters */

    printf("Crypto functions compiled but lack parameter validation\n");
    return 0;
}