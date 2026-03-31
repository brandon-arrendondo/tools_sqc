/*
 * Rule: FLP36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FLP36-C violation
 *
 * Using double preserves precision for long values
 */

#include <stdio.h>

void long_to_double(void) {
    long big = 1234567890L;
    /* COMPLIANT: double has ~15 decimal digits of precision */
    double precise = big;
    printf("Precise: %f\n", precise);
}
