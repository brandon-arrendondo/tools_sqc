/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Format string with large integers creates output exceeding buffer size
 */

#include <stdio.h>

int main() {
    char buffer[15];
    long long big_num = 9223372036854775807LL;  // Max long long

    sprintf(buffer, "Number: %lld", big_num);  // Output ~28 chars, buffer only 15
    printf("Formatted: %s\n", buffer);

    return 0;
}