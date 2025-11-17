/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input copied to buffer then used as format string
 */

#include <stdio.h>
#include <string.h>

int main() {
    char user_data[100];
    char format_buffer[100];

    printf("Enter format string: ");
    fgets(user_data, sizeof(user_data), stdin);

    // Copy user input to another buffer
    strcpy(format_buffer, user_data);

    // VULNERABLE: copied user input used as format
    printf(format_buffer);

    return 0;
}