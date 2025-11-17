/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Hash calculation with char type gives incorrect results
 */

#include <stdio.h>
#include <stdlib.h>

unsigned int simple_hash(FILE *file) {
    char c; // VIOLATION: char type skips 0xFF bytes in hash calculation
    unsigned int hash = 5381;

    // Hash will be incorrect for files containing 0xFF bytes
    while ((c = fgetc(file)) != EOF) {
        hash = ((hash << 5) + hash) + (unsigned char)c;
    }

    return hash;
}

int main() {
    FILE *file = fopen("input.dat", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    unsigned int hash = simple_hash(file);
    printf("File hash: 0x%08X\n", hash);

    fclose(file);
    return 0;
}