/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Buffer filling with char type loses data at 0xFF bytes
 */

#include <stdio.h>
#include <stdlib.h>

size_t fill_buffer(FILE *file, unsigned char *buffer, size_t size) {
    char c; // VIOLATION: char type causes incomplete buffer fills
    size_t i = 0;

    // Buffer will not be completely filled if input contains 0xFF bytes
    while (i < size && (c = fgetc(file)) != EOF) {
        buffer[i++] = (unsigned char)c;
    }

    return i;
}

int main() {
    FILE *file = fopen("binary_input.dat", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    unsigned char buffer[1024];
    size_t bytes_read = fill_buffer(file, buffer, sizeof(buffer));

    printf("Bytes read into buffer: %zu\n", bytes_read);

    // Print first few bytes
    for (size_t i = 0; i < bytes_read && i < 16; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");

    fclose(file);
    return 0;
}