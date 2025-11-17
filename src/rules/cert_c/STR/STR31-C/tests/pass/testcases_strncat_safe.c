/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Uses strncat with calculated remaining space to prevent overflow
 */

#include <stdio.h>
#include <string.h>

int main() {
    char dest[30] = "Hello";
    char source[] = " World";
    size_t remaining = sizeof(dest) - strlen(dest) - 1;

    strncat(dest, source, remaining);
    printf("Safely concatenated: %s\n", dest);

    return 0;
}