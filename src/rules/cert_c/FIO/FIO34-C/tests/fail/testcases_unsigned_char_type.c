/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Using unsigned char cannot represent EOF value (-1)
 */

#include <stdio.h>

int main() {
    unsigned char c; // VIOLATION: unsigned char cannot hold EOF

    printf("Reading input:\n");

    // This will never terminate normally because EOF (-1) cannot be
    // represented in unsigned char, so the comparison will always fail
    while ((c = getchar()) != EOF) {
        printf("Character: %c\n", c);
    }

    printf("This line may never be reached\n");
    return 0;
}