/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: No error checking with ferror() function
 */

#include <stdio.h>

int main() {
    int c;

    printf("Reading input without error checking:\n");

    // VIOLATION: Doesn't check ferror() for I/O errors vs EOF
    while ((c = getchar()) != EOF) {
        printf("Character: %c\n", c);
    }

    // Assumes EOF always means end of input, not I/O error
    printf("Reading completed\n");
    return 0;
}