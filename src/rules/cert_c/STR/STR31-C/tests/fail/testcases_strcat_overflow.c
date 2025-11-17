/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Buffer overflow when concatenating strings exceeds destination capacity
 */

#include <stdio.h>
#include <string.h>

int main() {
    char dest[15] = "Hello";
    char source[] = " World and Universe";  // Total would be 24 chars + null

    strcat(dest, source);  // Buffer overflow!
    printf("Result: %s\n", dest);

    return 0;
}