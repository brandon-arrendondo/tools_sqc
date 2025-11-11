/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Directly dereferencing a NULL pointer
 */

#include <stdio.h>

int main() {
    int *ptr = NULL;

    // Direct dereference of NULL pointer
    *ptr = 42;
    printf("Value: %d\n", *ptr);

    return 0;
}