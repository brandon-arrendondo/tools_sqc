/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: encryption_constants.c
 *
 * This case demonstrates violations where cryptographic constants
 * and security parameters are not const-qualified.
 */

#include <stdio.h>

void cipher_parameters(void) {
    /* NON-COMPLIANT: Cipher algorithm names should be const */
    char aes_128[] = "AES-128";
    char aes_256[] = "AES-256";
    char des_3[] = "3DES";
    char rsa_2048[] = "RSA-2048";
    char rsa_4096[] = "RSA-4096";

    /* NON-COMPLIANT: Key sizes should be const */
    int aes_key_size_128 = 128;
    int aes_key_size_192 = 192;
    int aes_key_size_256 = 256;
    int rsa_key_size_2048 = 2048;
    int rsa_key_size_4096 = 4096;

    /* NON-COMPLIANT: Block sizes should be const */
    int aes_block_size = 128;
    int des_block_size = 64;
    int sha1_block_size = 512;
    int sha256_block_size = 512;

    printf("Cipher Parameters:\\n");
    printf("  Algorithms: %s, %s, %s, %s, %s\\n",
           aes_128, aes_256, des_3, rsa_2048, rsa_4096);
    printf("  AES key sizes: %d, %d, %d bits\\n",
           aes_key_size_128, aes_key_size_192, aes_key_size_256);
    printf("  RSA key sizes: %d, %d bits\\n", rsa_key_size_2048, rsa_key_size_4096);
    printf("  Block sizes: AES=%d, DES=%d, SHA1=%d, SHA256=%d bits\\n",
           aes_block_size, des_block_size, sha1_block_size, sha256_block_size);

    /* Parameters used for algorithm selection but never modified */
    int selected_key_size = aes_key_size_256;
    printf("  Selected key size: %d bits\\n", selected_key_size);
}

void hash_algorithms(void) {
    /* NON-COMPLIANT: Hash algorithm names should be const */
    char md5_name[] = "MD5";
    char sha1_name[] = "SHA-1";
    char sha256_name[] = "SHA-256";
    char sha384_name[] = "SHA-384";
    char sha512_name[] = "SHA-512";

    /* NON-COMPLIANT: Hash output sizes should be const */
    int md5_size = 128;        /* bits */
    int sha1_size = 160;
    int sha256_size = 256;
    int sha384_size = 384;
    int sha512_size = 512;

    /* NON-COMPLIANT: Hash output sizes in bytes should be const */
    int md5_bytes = 16;
    int sha1_bytes = 20;
    int sha256_bytes = 32;
    int sha384_bytes = 48;
    int sha512_bytes = 64;

    printf("\\nHash Algorithms:\\n");
    printf("  Algorithms: %s, %s, %s, %s, %s\\n",
           md5_name, sha1_name, sha256_name, sha384_name, sha512_name);
    printf("  Output sizes (bits): MD5=%d, SHA1=%d, SHA256=%d\\n",
           md5_size, sha1_size, sha256_size);
    printf("  Output sizes (bytes): MD5=%d, SHA1=%d, SHA256=%d\\n",
           md5_bytes, sha1_bytes, sha256_bytes);

    /* Hash sizes used for buffer allocation but never modified */
    int buffer_size = sha256_bytes;
    printf("  Hash buffer size: %d bytes\\n", buffer_size);
}

void ssl_tls_parameters(void) {
    /* NON-COMPLIANT: SSL/TLS version constants should be const */
    char ssl_v3[] = "SSLv3";
    char tls_v1_0[] = "TLSv1.0";
    char tls_v1_1[] = "TLSv1.1";
    char tls_v1_2[] = "TLSv1.2";
    char tls_v1_3[] = "TLSv1.3";

    /* NON-COMPLIANT: Protocol version numbers should be const */
    int ssl3_version = 0x0300;
    int tls10_version = 0x0301;
    int tls11_version = 0x0302;
    int tls12_version = 0x0303;
    int tls13_version = 0x0304;

    /* NON-COMPLIANT: Cipher suite names should be const */
    char cipher_aes_128_gcm[] = "AES128-GCM-SHA256";
    char cipher_aes_256_gcm[] = "AES256-GCM-SHA384";
    char cipher_chacha20[] = "CHACHA20-POLY1305-SHA256";

    printf("\\nSSL/TLS Configuration:\\n");
    printf("  Versions: %s, %s, %s, %s, %s\\n",
           ssl_v3, tls_v1_0, tls_v1_1, tls_v1_2, tls_v1_3);
    printf("  Version codes: 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X\\n",
           ssl3_version, tls10_version, tls11_version, tls12_version, tls13_version);
    printf("  Cipher suites: %s, %s, %s\\n",
           cipher_aes_128_gcm, cipher_aes_256_gcm, cipher_chacha20);

    /* TLS versions used for negotiation but never modified */
    int min_version = tls12_version;
    printf("  Minimum TLS version: 0x%04X\\n", min_version);
}

void key_derivation(void) {
    /* NON-COMPLIANT: KDF algorithm names should be const */
    char pbkdf2_name[] = "PBKDF2";
    char scrypt_name[] = "scrypt";
    char argon2_name[] = "Argon2";
    char hkdf_name[] = "HKDF";

    /* NON-COMPLIANT: KDF parameters should be const */
    int pbkdf2_iterations = 100000;
    int scrypt_n = 16384;
    int scrypt_r = 8;
    int scrypt_p = 1;
    int argon2_memory = 65536;
    int argon2_iterations = 3;
    int argon2_parallelism = 4;

    /* NON-COMPLIANT: Salt sizes should be const */
    int min_salt_size = 16;    /* bytes */
    int recommended_salt_size = 32;
    int max_salt_size = 64;

    printf("\\nKey Derivation Functions:\\n");
    printf("  Algorithms: %s, %s, %s, %s\\n",
           pbkdf2_name, scrypt_name, argon2_name, hkdf_name);
    printf("  PBKDF2 iterations: %d\\n", pbkdf2_iterations);
    printf("  Scrypt parameters: N=%d, r=%d, p=%d\\n", scrypt_n, scrypt_r, scrypt_p);
    printf("  Argon2 parameters: memory=%dKB, iterations=%d, parallelism=%d\\n",
           argon2_memory, argon2_iterations, argon2_parallelism);
    printf("  Salt sizes: min=%d, recommended=%d, max=%d bytes\\n",
           min_salt_size, recommended_salt_size, max_salt_size);

    /* KDF parameters used for key generation but never modified */
    int selected_iterations = pbkdf2_iterations;
    printf("  Selected iterations: %d\\n", selected_iterations);
}

int main(void) {
    /* NON-COMPLIANT: Security levels should be const */
    int security_level_low = 80;      /* bits */
    int security_level_medium = 112;
    int security_level_high = 128;
    int security_level_very_high = 192;
    int security_level_ultra = 256;

    /* NON-COMPLIANT: Cryptographic constants should be const */
    int random_seed_size = 32;         /* bytes */
    int nonce_size = 12;
    int tag_size = 16;
    int iv_size = 16;

    printf("Security Levels (bits):\\n");
    printf("  Low: %d, Medium: %d, High: %d, Very High: %d, Ultra: %d\\n",
           security_level_low, security_level_medium, security_level_high,
           security_level_very_high, security_level_ultra);

    printf("\\nCryptographic Sizes (bytes):\\n");
    printf("  Random seed: %d, Nonce: %d, Tag: %d, IV: %d\\n",
           random_seed_size, nonce_size, tag_size, iv_size);

    cipher_parameters();
    hash_algorithms();
    ssl_tls_parameters();
    key_derivation();

    return 0;
}