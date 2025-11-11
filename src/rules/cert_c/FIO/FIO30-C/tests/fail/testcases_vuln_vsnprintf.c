/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in vsnprintf
 */

#include <stdio.h>
#include <stdarg.h>

void safe_format(char *buffer, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // VULNERABLE: format parameter could be user input
    vsnprintf(buffer, size, format, args);

    va_end(args);
}

int main() {
    char user_fmt[100];
    char result[200];

    printf("Enter format: ");
    fgets(user_fmt, sizeof(user_fmt), stdin);

    // VULNERABLE: user input passed as format
    safe_format(result, sizeof(result), user_fmt);
    printf("Result: %s\n", result);

    return 0;
}