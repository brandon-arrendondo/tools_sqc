/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Buffer is large enough to accommodate formatted string output
 */

#include <stdio.h>

int main() {
    char buffer[50];  // Large enough for formatted output
    int value = 42;
    char name[] = "Test";

    sprintf(buffer, "Value: %d, Name: %s", value, name);
    printf("Formatted: %s\n", buffer);

    return 0;
}