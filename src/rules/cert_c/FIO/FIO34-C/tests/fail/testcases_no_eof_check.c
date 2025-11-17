/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: No proper EOF checking with feof/ferror functions
 */

#include <stdio.h>

int main() {
    int c;

    printf("Reading characters:\n");

    // VIOLATION: Only checks for EOF, doesn't use feof/ferror
    // May not properly handle all EOF conditions
    while ((c = getchar()) != EOF) {
        printf("Character: %c\n", c);
    }

    // Assumes EOF means end of file, but doesn't verify
    printf("Done reading\n");
    return 0;
}