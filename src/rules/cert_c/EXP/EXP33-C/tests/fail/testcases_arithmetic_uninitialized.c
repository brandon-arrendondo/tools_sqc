/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: arithmetic_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Arithmetic with uninitialized operands */
void unsafe_arithmetic(void) {
    int a, b, c;  /* All uninitialized */

    a = 5;
    /* b and c remain uninitialized */

    int sum = a + b;      /* Undefined behavior */
    int product = b * c;  /* Undefined behavior */
    int difference = a - c; /* Undefined behavior */

    printf("Sum: %d, Product: %d, Difference: %d\n", sum, product, difference);
}

int main(void) {
    unsafe_arithmetic();
    return 0;
}