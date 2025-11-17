/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: User input length not validated before copying to fixed buffer
 */

#include <stdio.h>
#include <string.h>

int main() {
    char input[256];
    char name[10];

    printf("Enter your full name: ");
    fgets(input, sizeof(input), stdin);

    // Remove newline but don't check length
    input[strcspn(input, "\n")] = '\0';

    strcpy(name, input);  // Input might be longer than 10 chars
    printf("Name: %s\n", name);

    return 0;
}