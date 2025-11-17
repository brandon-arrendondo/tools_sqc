/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input directly used as format string in printf
 */

#include <stdio.h>

int main() {
    char user_input[100];

    printf("Enter a format string: ");
    fgets(user_input, sizeof(user_input), stdin);

    // VULNERABLE: user input used directly as format string
    printf(user_input);

    return 0;
}