/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Pointer to user input used as format string
 */

#include <stdio.h>

int main() {
    char user_buffer[100];
    char *format_ptr;

    printf("Enter format: ");
    fgets(user_buffer, sizeof(user_buffer), stdin);

    format_ptr = user_buffer;

    // VULNERABLE: pointer to user input used as format
    printf(format_ptr);

    return 0;
}