/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in scanf
 */

#include <stdio.h>

int main() {
    char scan_format[50];
    char data[100];

    printf("Enter scanf format: ");
    fgets(scan_format, sizeof(scan_format), stdin);

    printf("Enter data: ");
    // VULNERABLE: user input as scanf format string
    scanf(scan_format, data);

    printf("Scanned: %s\n", data);
    return 0;
}