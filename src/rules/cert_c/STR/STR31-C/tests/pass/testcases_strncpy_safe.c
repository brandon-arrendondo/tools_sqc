/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Uses strncpy with proper buffer size and ensures null termination
 */

#include <stdio.h>
#include <string.h>

int main() {
    char source[] = "This is a longer string";
    char dest[15];

    strncpy(dest, source, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';  // Ensure null termination

    printf("Safely copied: %s\n", dest);

    return 0;
}