/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input passed as format string to vprintf
 */

#include <stdio.h>
#include <stdarg.h>

void vulnerable_log(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // VULNERABLE: format parameter could be user input
    vprintf(format, args);

    va_end(args);
}

int main() {
    char user_format[100];

    printf("Enter log format: ");
    fgets(user_format, sizeof(user_format), stdin);

    // VULNERABLE: user input as format string
    vulnerable_log(user_format);

    return 0;
}