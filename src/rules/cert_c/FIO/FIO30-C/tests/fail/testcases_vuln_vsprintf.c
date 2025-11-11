/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in vsprintf
 */

#include <stdio.h>
#include <stdarg.h>

void create_message(char *buffer, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // VULNERABLE: format could contain user input
    vsprintf(buffer, format, args);

    va_end(args);
}

int main() {
    char message_format[100];
    char output[200];

    printf("Enter message format: ");
    fgets(message_format, sizeof(message_format), stdin);

    // VULNERABLE: user input as format string
    create_message(output, message_format);
    printf("Message: %s\n", output);

    return 0;
}