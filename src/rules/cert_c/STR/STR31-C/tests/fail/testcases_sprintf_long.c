/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: sprintf output exceeds buffer size causing overflow
 */

#include <stdio.h>

int main() {
    char buffer[10];
    char name[] = "Alexander";
    int value = 123456789;

    sprintf(buffer, "Name: %s, Value: %d", name, value);  // Output ~25 chars, buffer only 10
    printf("Result: %s\n", buffer);

    return 0;
}