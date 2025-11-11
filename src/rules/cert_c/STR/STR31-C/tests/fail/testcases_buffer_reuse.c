/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Reusing buffer without considering accumulated length
 */

#include <stdio.h>
#include <string.h>

int main() {
    char buffer[15] = "Start";

    // Each operation assumes buffer is empty
    strcat(buffer, " middle");  // Now "Start middle" (12 chars)
    strcat(buffer, " end");     // Now tries to add 4 more chars to 12
    strcat(buffer, " more");    // Buffer overflow!

    printf("Result: %s\n", buffer);

    return 0;
}