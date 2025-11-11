/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Dynamically constructed format string includes user input
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char user_prefix[50];
    char *dynamic_format;

    printf("Enter log prefix: ");
    scanf("%49s", user_prefix);

    // Construct format string with user input
    dynamic_format = malloc(100);
    strcpy(dynamic_format, user_prefix);
    strcat(dynamic_format, ": %s\n");

    // VULNERABLE: format string contains user input
    printf(dynamic_format, "message");

    free(dynamic_format);
    return 0;
}