/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: scanf with %s can read more data than buffer can hold
 */

#include <stdio.h>

int main() {
    char name[10];

    printf("Enter your name: ");
    scanf("%s", name);  // No length limit - can overflow buffer
    printf("Hello, %s!\n", name);

    return 0;
}