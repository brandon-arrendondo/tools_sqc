/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Hex dump utility with char type misses 0xFF bytes
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("binary.dat", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    char c; // VIOLATION: char type will skip 0xFF bytes
    size_t offset = 0;

    printf("Hex dump:\n");
    printf("Offset  Hex\n");

    // This hex dump will be incomplete - missing all 0xFF bytes
    while ((c = fgetc(file)) != EOF) {
        printf("%06zx  %02x\n", offset++, (unsigned char)c);
    }

    printf("Total bytes shown: %zu\n", offset);
    fclose(file);
    return 0;
}