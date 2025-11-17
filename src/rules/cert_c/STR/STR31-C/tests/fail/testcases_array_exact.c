/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Array size matches string length exactly, no space for null terminator
 */

#include <stdio.h>
#include <string.h>

int main() {
    char message[5];  // Exactly 5 chars, no space for null terminator
    char source[] = "Hello";  // 5 characters

    strcpy(message, source);  // No space for null terminator!
    printf("Message: %s\n", message);

    return 0;
}