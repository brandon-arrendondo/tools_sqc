/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Dereferencing NULL pointer in switch statement
 */

#include <stdio.h>

int main() {
    int *ptr = NULL;

    // Dereferencing NULL in switch expression
    switch (*ptr) {
        case 0:
            printf("Zero\n");
            break;
        default:
            printf("Other\n");
            break;
    }

    return 0;
}