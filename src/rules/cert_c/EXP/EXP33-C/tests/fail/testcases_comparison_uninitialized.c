/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: comparison_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Comparisons with uninitialized variables */
void unsafe_comparisons(void) {
    int a, b;  /* Uninitialized */

    if (a > b) {           /* Undefined behavior */
        printf("a is greater\n");
    } else if (a < b) {    /* Undefined behavior */
        printf("b is greater\n");
    } else {
        printf("equal\n");
    }
}

int main(void) {
    unsafe_comparisons();
    return 0;
}