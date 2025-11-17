/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: recursive_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Recursive function with uninitialized accumulator */
int unsafe_factorial(int n) {
    static int accumulator;  /* Uninitialized static */

    if (n <= 1) {
        return accumulator * 1;  /* Uses uninitialized static */
    }
    accumulator *= n;
    return unsafe_factorial(n - 1);
}

int main(void) {
    printf("Factorial: %d\n", unsafe_factorial(5));
    return 0;
}