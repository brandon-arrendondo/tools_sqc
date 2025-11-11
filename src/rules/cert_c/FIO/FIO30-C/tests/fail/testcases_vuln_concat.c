/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input concatenated into format string
 */

#include <stdio.h>
#include <string.h>

int main() {
    char prefix[50];
    char format_string[100];

    printf("Enter message prefix: ");
    fgets(prefix, sizeof(prefix), stdin);

    // Remove newline
    prefix[strcspn(prefix, "\n")] = 0;

    // Concatenate user input with format specifiers
    strcpy(format_string, prefix);
    strcat(format_string, ": %s\n");

    // VULNERABLE: format string contains user input
    printf(format_string, "message");

    return 0;
}