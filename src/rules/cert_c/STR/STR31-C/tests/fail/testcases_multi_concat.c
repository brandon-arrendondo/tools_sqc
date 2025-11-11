/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Multiple concatenations exceed buffer capacity
 */

#include <stdio.h>
#include <string.h>

int main() {
    char buffer[20] = "Start";
    char part1[] = " middle";
    char part2[] = " and end";
    char part3[] = " extra";

    strcat(buffer, part1);
    strcat(buffer, part2);
    strcat(buffer, part3);  // Total length exceeds 20 bytes
    printf("Result: %s\n", buffer);

    return 0;
}