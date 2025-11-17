/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in vfprintf
 */

#include <stdio.h>
#include <stdarg.h>

void log_to_file(FILE *file, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // VULNERABLE: format could be user-controlled
    vfprintf(file, format, args);

    va_end(args);
}

int main() {
    char user_template[100];
    FILE *log_file = fopen("log.txt", "w");

    printf("Enter log template: ");
    fgets(user_template, sizeof(user_template), stdin);

    if (log_file) {
        // VULNERABLE: user input as format
        log_to_file(log_file, user_template);
        fclose(log_file);
    }

    return 0;
}