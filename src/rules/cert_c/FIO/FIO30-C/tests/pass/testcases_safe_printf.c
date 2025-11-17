/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses literal format string with proper specifiers for user data
 */

#include <stdio.h>

int main() {
    char user_input[100];
    printf("Enter your name: ");

    if (fgets(user_input, sizeof(user_input), stdin)) {
        // Safe: literal format string, user input as argument
        printf("Hello, %s!\n", user_input);
    }

    return 0;
}