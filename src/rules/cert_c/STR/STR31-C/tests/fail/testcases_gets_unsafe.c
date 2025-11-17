/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: gets() does not check buffer bounds and can cause overflow
 */

#include <stdio.h>

int main() {
    char buffer[20];

    printf("Enter text: ");
    gets(buffer);  // Dangerous! No bounds checking - user can overflow buffer
    printf("You entered: %s\n", buffer);

    return 0;
}