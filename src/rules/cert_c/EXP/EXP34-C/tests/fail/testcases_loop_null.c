/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Dereferencing NULL pointer in loop condition
 */

#include <stdio.h>

int main() {
    int *ptr = NULL;

    // Dereferencing NULL in loop condition
    while (*ptr < 10) {
        printf("Value: %d\n", *ptr);
        (*ptr)++;
    }

    return 0;
}