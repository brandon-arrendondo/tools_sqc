/*
 * Rule: FLP03-C
 * Source: testcases
 * Status: FAIL - Floating-point division without any guard or fenv checking.
 */

void unguarded_division(void) {
    double a = 3.14;
    double b = 0.0;
    double result = a / b;
    (void)result;
}
