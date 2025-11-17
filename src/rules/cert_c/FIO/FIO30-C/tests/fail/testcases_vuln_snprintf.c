/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User-controlled string used as format in snprintf
 */

#include <stdio.h>

int main() {
    char output[100];
    char format_str[50];

    printf("Enter format: ");
    scanf("%49s", format_str);

    // VULNERABLE: user-controlled format string
    snprintf(output, sizeof(output), format_str);
    printf("Output: %s\n", output);

    return 0;
}