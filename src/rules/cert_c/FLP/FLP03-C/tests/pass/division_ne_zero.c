/*
 * Rule: FLP03-C
 * Source: testcases
 * Status: PASS - Division inside `if (x != 0)` guard.
 */

void ne_zero_guarded_division(void) {
    double a = 3.14;
    double b = 0.001;
    if (b != 0) {
        double result = a / b;
        (void)result;
    }
}
