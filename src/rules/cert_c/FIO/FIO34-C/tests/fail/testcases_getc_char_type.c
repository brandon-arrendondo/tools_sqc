/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Using char type makes 255 (0xFF) indistinguishable from EOF
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("test.dat", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    char c; // VIOLATION: cannot properly distinguish EOF from valid bytes

    printf("Processing file contents:\n");
    while ((c = getc(file)) != EOF) {
        // This will fail to read bytes with value 0xFF correctly
        // as they will be sign-extended to -1 (EOF value)
        printf("Read byte: %d\n", c);
    }

    fclose(file);
    return 0;
}