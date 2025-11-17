/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Pointer arithmetic on NULL pointer
 */

#include <stdio.h>

int main() {
    int *ptr = NULL;

    // Pointer arithmetic on NULL
    ptr++;
    *ptr = 42;

    printf("Value: %d\n", *ptr);

    return 0;
}