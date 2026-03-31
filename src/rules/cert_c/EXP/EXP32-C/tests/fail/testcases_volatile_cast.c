/*
 * Rule: EXP32-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP32-C violation
 * Description: Non-volatile pointer accessed through volatile pointer-to-pointer
 */

#include <stdio.h>

void volatile_mismatch(void) {
    static volatile int **vpp;
    static int *np;
    static volatile int val = 10;

    vpp = &np;    /* Violation: np is non-volatile pointer */
    *vpp = &val;
}
