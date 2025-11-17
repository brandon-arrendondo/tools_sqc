/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Character counting with char type gives incorrect results
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("data.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    char c; // VIOLATION: char type causes incorrect counting
    size_t count = 0;

    // Will undercount if file contains bytes with value 0xFF
    while ((c = fgetc(file)) != EOF) {
        count++;
    }

    printf("Characters counted: %zu\n", count);

    fclose(file);
    return 0;
}