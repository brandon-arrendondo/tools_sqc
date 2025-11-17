/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: strncpy may not null-terminate if source length equals or exceeds count
 */

#include <stdio.h>
#include <string.h>

int main() {
    char source[] = "Exactly10!";  // 10 characters
    char dest[10];

    strncpy(dest, source, 10);  // No space for null terminator
    // dest is not null-terminated!
    printf("Copied: %s\n", dest);  // Undefined behavior

    return 0;
}