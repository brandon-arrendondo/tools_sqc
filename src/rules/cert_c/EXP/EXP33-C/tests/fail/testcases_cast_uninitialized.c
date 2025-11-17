/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: cast_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Type casting with uninitialized values */
void unsafe_casting(void) {
    int int_val;      /* Uninitialized */
    float float_val;  /* Uninitialized */

    double result1 = (double)int_val;      /* Cast uninitialized int */
    int result2 = (int)float_val;          /* Cast uninitialized float */

    printf("Results: %f, %d\n", result1, result2);
}

int main(void) {
    unsafe_casting();
    return 0;
}