/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses fputs instead of printf to avoid format string interpretation
 */

#include <stdio.h>

int main() {
    char user_message[200];

    printf("Enter a message: ");
    fgets(user_message, sizeof(user_message), stdin);

    printf("You entered: ");
    // Safe: fputs doesn't interpret format strings
    fputs(user_message, stdout);

    // Alternative safe approach: using %s format specifier
    printf("Message again: %s", user_message);

    return 0;
}