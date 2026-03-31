/*
 * Rule: FLP03-C
 * Source: testcases
 * Status: PASS - Division inside `if (x > 0)` guard (sign check implies non-zero).
 */

void gt_zero_guarded_division(void) {
    double a = 3.14;
    double b = 1.0;
    if (b > 0) {
        double result = a / b;
        (void)result;
    }
}
