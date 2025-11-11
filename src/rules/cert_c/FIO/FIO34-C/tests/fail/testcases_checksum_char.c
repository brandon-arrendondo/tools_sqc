/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Checksum calculation with char type gives incorrect results
 */

#include <stdio.h>
#include <stdlib.h>

unsigned long calculate_checksum(FILE *file) {
    char c; // VIOLATION: char type skips 0xFF bytes
    unsigned long checksum = 0;
    size_t count = 0;

    // Checksum will be incorrect for files containing 0xFF bytes
    while ((c = fgetc(file)) != EOF) {
        checksum += (unsigned char)c;
        count++;
    }

    printf("Processed %zu bytes for checksum\n", count);
    return checksum;
}

int main() {
    FILE *file = fopen("data.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    unsigned long checksum = calculate_checksum(file);
    printf("File checksum: %lu\n", checksum);

    fclose(file);
    return 0;
}