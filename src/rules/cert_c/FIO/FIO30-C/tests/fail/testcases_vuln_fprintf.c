/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in fprintf
 */

#include <stdio.h>

int main() {
    char log_format[100];
    FILE *log_file = fopen("output.log", "w");

    printf("Enter log format: ");
    fgets(log_format, sizeof(log_format), stdin);

    if (log_file) {
        // VULNERABLE: user input as format string
        fprintf(log_file, log_format);
        fclose(log_file);
    }

    return 0;
}