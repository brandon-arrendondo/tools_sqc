/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Stream cipher with char type loses encrypted bytes
 */

#include <stdio.h>
#include <stdlib.h>

void simple_cipher(FILE *input, FILE *output, unsigned char key) {
    char c; // VIOLATION: char type cannot handle all encrypted values

    // Encryption will lose data when encrypted bytes equal 0xFF
    while ((c = fgetc(input)) != EOF) {
        unsigned char encrypted = (unsigned char)c ^ key;
        fputc(encrypted, output);
    }
}

int main() {
    FILE *input = fopen("plaintext.txt", "rb");
    FILE *output = fopen("encrypted.bin", "wb");

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Could not open files\n");
        return 1;
    }

    unsigned char cipher_key = 0x5A;
    simple_cipher(input, output, cipher_key);

    printf("Encryption completed\n");

    fclose(input);
    fclose(output);
    return 0;
}