/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses puts() which doesn't interpret format strings
 */

#include <stdio.h>

int main() {
    char user_input[100];

    printf("Enter some text: ");
    fgets(user_input, sizeof(user_input), stdin);

    printf("Safe output methods:\n");

    // Safe: puts doesn't interpret format strings
    puts("1. Using puts:");
    puts(user_input);

    // Safe: printf with %s format specifier
    printf("2. Using printf with %%s: %s", user_input);

    return 0;
}