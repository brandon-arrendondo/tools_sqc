/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Dereferencing NULL in conditional expression
 */

#include <stdio.h>

int main() {
    int *ptr = NULL;

    // Dereferencing NULL in conditional
    if (*ptr == 0) {
        printf("Pointer is zero\n");
    }

    return 0;
}