/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: ternary_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Ternary operator with uninitialized operands */
void unsafe_ternary(void) {
    int a, b, condition;  /* All uninitialized */

    int result = condition ? a : b;  /* All operands uninitialized */
    printf("Result: %d\n", result);
}

int main(void) {
    unsafe_ternary();
    return 0;
}