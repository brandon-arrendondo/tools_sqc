/*
 * Rule: FLP03-C
 * Source: testcases
 * Status: PASS - Division inside fabs() magnitude guard.
 */

#include <math.h>

void fabs_guarded_division(void) {
    double a = 3.14;
    double b = 0.001;
    if (fabs(b) > 0.000001) {
        double result = a / b;
        (void)result;
    }
}
