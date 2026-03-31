/*
 * Rule: EXP32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP32-C violation
 * Description: Consistent volatile qualifiers throughout pointer chain
 */

void volatile_consistent(void) {
    static volatile int **vpp;
    static volatile int *vp;
    static volatile int val = 42;

    vpp = &vp;
    *vpp = &val;

    if (*vp != 0) {
        /* safe access */
    }
}
