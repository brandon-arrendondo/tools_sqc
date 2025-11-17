/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: register_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Register variable uninitialized */
void unsafe_register(void) {
    register int fast_var;  /* Uninitialized register variable */

    printf("Register variable: %d\n", fast_var);  /* Undefined behavior */
}

int main(void) {
    unsafe_register();
    return 0;
}