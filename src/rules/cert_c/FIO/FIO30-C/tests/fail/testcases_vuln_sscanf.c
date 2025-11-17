/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in sscanf
 */

#include <stdio.h>

int main() {
    char input_data[] = "123 456 hello";
    char scan_format[50];
    int a, b;
    char str[20];

    printf("Enter sscanf format: ");
    fgets(scan_format, sizeof(scan_format), stdin);

    // VULNERABLE: user input as sscanf format
    sscanf(input_data, scan_format, &a, &b, str);

    printf("Parsed: %d, %d, %s\n", a, b, str);
    return 0;
}