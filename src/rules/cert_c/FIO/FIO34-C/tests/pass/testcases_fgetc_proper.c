/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Uses int for character storage and validates with feof/ferror
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("test_input.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    int c; // Correct: using int to distinguish from EOF

    printf("Reading file contents:\n");
    while ((c = fgetc(file)) != EOF || (!feof(file) && !ferror(file))) {
        if (c != EOF) {
            if (c >= 32 && c <= 126) {
                printf("Character: '%c' (ASCII: %d)\n", c, c);
            } else {
                printf("Non-printable character (ASCII: %d)\n", c);
            }
        }
    }

    if (feof(file)) {
        printf("Successfully reached end of file\n");
    }
    if (ferror(file)) {
        fprintf(stderr, "Error reading from file\n");
    }

    fclose(file);
    return 0;
}