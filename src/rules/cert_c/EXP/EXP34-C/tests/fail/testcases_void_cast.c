/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Casting NULL to pointer type and dereferencing
 */

#include <stdio.h>

int main() {
    int *ptr = (int*)NULL;

    // Dereferencing explicitly cast NULL pointer
    *ptr = 42;
    printf("Value: %d\n", *ptr);

    return 0;
}