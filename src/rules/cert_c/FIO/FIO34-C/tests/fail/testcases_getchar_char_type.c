/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Using char instead of int - cannot distinguish between 0xFF and EOF
 */

#include <stdio.h>

int main() {
    char c; // VIOLATION: char cannot hold EOF value properly

    printf("Reading characters from stdin:\n");

    // This loop may terminate prematurely if a character with value 0xFF is read
    // because it would be interpreted as EOF when sign-extended
    while ((c = getchar()) != EOF) {
        printf("Character: %c (value: %d)\n", c, c);
    }

    printf("End of input reached\n");
    return 0;
}