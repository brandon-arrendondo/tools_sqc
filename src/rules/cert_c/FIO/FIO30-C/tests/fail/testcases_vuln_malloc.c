/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Dynamically allocated string with user input used as format
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char user_input[100];
    char *dynamic_format;

    printf("Enter format: ");
    fgets(user_input, sizeof(user_input), stdin);

    // Allocate and copy user input
    dynamic_format = malloc(strlen(user_input) + 1);
    strcpy(dynamic_format, user_input);

    // VULNERABLE: dynamically allocated user input as format
    printf(dynamic_format);

    free(dynamic_format);
    return 0;
}