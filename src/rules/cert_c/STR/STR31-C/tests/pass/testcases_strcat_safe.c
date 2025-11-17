/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Buffer is sized to accommodate concatenated strings plus null terminator
 */

#include <stdio.h>
#include <string.h>

int main() {
    char first[] = "Hello";
    char second[] = " World";
    char result[20];  // 5 + 6 + 1 for null terminator = 12, so 20 is sufficient

    strcpy(result, first);
    strcat(result, second);
    printf("Concatenated: %s\n", result);

    return 0;
}