/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Uses int for character storage and proper EOF checking with feof/ferror
 */

#include <stdio.h>

int main() {
    int c; // Correct: using int, not char

    printf("Reading characters from stdin until EOF:\n");

    while ((c = getchar()) != EOF || (!feof(stdin) && !ferror(stdin))) {
        if (c != EOF) {
            printf("Character: %c (value: %d)\n", c, c);
        }
    }

    if (feof(stdin)) {
        printf("End of file reached\n");
    }
    if (ferror(stdin)) {
        printf("Error occurred while reading\n");
    }

    return 0;
}