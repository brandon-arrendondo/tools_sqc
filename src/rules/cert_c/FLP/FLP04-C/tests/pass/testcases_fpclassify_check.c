/*
 * Rule: FLP04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FLP04-C violation
 *
 * Float input validated with fpclassify()
 */

#include <stdio.h>
#include <math.h>

void fpclassify_validation(void) {
    double val;
    scanf("%lf", &val);

    /* COMPLIANT: fpclassify checks for all exceptional classes */
    if (fpclassify(val) == FP_NAN || fpclassify(val) == FP_INFINITE) {
        fprintf(stderr, "Invalid input\n");
        return;
    }
    double result = val + 1.0;
    printf("Result: %f\n", result);
}
