/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using uninitialized pointer (may contain NULL)
 */

#include <stdio.h>

int main() {
    int *ptr;  // Uninitialized pointer

    // Using uninitialized pointer
    *ptr = 42;
    printf("Value: %d\n", *ptr);

    return 0;
}