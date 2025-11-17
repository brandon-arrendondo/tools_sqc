/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Buffer is properly sized to accommodate the source string plus null terminator
 */

#include <stdio.h>
#include <string.h>

int main() {
    char source[] = "Hello, World!";
    char dest[20];  // Sufficient space for source string (13 chars) + null terminator

    strcpy(dest, source);
    printf("Copied string: %s\n", dest);

    return 0;
}