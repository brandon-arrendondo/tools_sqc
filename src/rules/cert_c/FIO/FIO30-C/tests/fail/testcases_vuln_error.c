/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User error message used as format string
 */

#include <stdio.h>

void report_error(const char *error_msg) {
    // VULNERABLE: error message could contain format specifiers
    fprintf(stderr, error_msg);
    fprintf(stderr, "\n");
}

int main() {
    char user_error[100];

    printf("Enter error message: ");
    fgets(user_error, sizeof(user_error), stdin);

    // VULNERABLE: user input as format string
    report_error(user_error);

    return 0;
}