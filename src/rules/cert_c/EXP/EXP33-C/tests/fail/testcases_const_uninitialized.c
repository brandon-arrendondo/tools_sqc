/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: const_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Const variable potentially uninitialized */
void unsafe_const_usage(int condition) {
    const int value;  /* Uninitialized const */

    if (condition) {
        /* value should be initialized here but isn't */
    }

    printf("Const value: %d\n", value);  /* Reading uninitialized const */
}

int main(void) {
    unsafe_const_usage(0);
    return 0;
}