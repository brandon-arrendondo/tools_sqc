/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Destination buffer is too small to hold the source string plus null terminator
 */

#include <stdio.h>
#include <string.h>

int main() {
    char source[] = "This string is too long";
    char dest[10];  // Only 10 bytes, but source needs 24 bytes including null terminator

    strcpy(dest, source);  // Buffer overflow!
    printf("Copied: %s\n", dest);

    return 0;
}