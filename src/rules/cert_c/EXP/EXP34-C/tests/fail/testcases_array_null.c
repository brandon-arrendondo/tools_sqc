/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Accessing array elements through NULL pointer
 */

#include <stdio.h>

int main() {
    int *arr = NULL;

    // Accessing array element through NULL pointer
    arr[0] = 10;
    arr[1] = 20;

    printf("First element: %d\n", arr[0]);

    return 0;
}