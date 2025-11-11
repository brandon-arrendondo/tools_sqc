/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in sprintf
 */

#include <stdio.h>

int main() {
    char buffer[200];
    char user_format[100];

    printf("Enter format string: ");
    fgets(user_format, sizeof(user_format), stdin);

    // VULNERABLE: user input as format string
    sprintf(buffer, user_format);
    printf("Result: %s\n", buffer);

    return 0;
}