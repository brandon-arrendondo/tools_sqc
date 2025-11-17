/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Macro expansion creates longer string than buffer can hold
 */

#include <stdio.h>
#include <string.h>

#define PREFIX "SYSTEM_ERROR_"
#define SUFFIX "_CRITICAL"

int main() {
    char buffer[15];
    char code[] = "404";

    strcpy(buffer, PREFIX);    // "SYSTEM_ERROR_" (13 chars)
    strcat(buffer, code);      // "SYSTEM_ERROR_404" (16 chars) - already overflow
    strcat(buffer, SUFFIX);    // Tries to add "_CRITICAL" (9 more chars)

    printf("Error: %s\n", buffer);

    return 0;
}