/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Assigning NULL then immediately dereferencing
 */

#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    *ptr = 10;

    ptr = NULL;  // Explicitly setting to NULL

    // Immediately dereferencing after setting to NULL
    printf("Value: %d\n", *ptr);

    return 0;
}