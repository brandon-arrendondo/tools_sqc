/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Function parameter used as format string without validation
 */

#include <stdio.h>

void print_message(const char *fmt) {
    // VULNERABLE: parameter used directly as format string
    printf(fmt);
}

int main() {
    char user_format[100];

    printf("Enter message format: ");
    fgets(user_format, sizeof(user_format), stdin);

    // VULNERABLE: user input passed to function as format
    print_message(user_format);

    return 0;
}