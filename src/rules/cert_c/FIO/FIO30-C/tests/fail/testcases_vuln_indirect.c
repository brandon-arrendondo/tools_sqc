/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input indirectly used as format string through variable
 */

#include <stdio.h>

int main() {
    char user_input[100];
    char *format_ptr;

    printf("Enter format: ");
    fgets(user_input, sizeof(user_input), stdin);

    // Indirect assignment
    format_ptr = user_input;

    // VULNERABLE: indirectly using user input as format
    printf(format_ptr);

    return 0;
}