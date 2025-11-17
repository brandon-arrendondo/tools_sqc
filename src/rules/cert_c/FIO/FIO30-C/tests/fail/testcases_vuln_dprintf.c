/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in dprintf
 */

#include <stdio.h>
#include <unistd.h>

int main() {
    char format_str[100];

    printf("Enter format for file descriptor output: ");
    fgets(format_str, sizeof(format_str), stdin);

    // VULNERABLE: user input as dprintf format
    dprintf(STDOUT_FILENO, format_str);

    return 0;
}