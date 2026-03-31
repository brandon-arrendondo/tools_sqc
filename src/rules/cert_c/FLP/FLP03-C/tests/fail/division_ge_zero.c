/*
 * Rule: FLP03-C
 * Source: testcases
 * Status: FAIL - Division inside `if (x >= 0)` does NOT exclude zero.
 *         The guard must use > 0 or != 0 to be effective.
 */

void division_ge_zero(void) {
    double a = 3.14;
    double b = 0.0;
    if (b >= 0) {
        double result = a / b;
        (void)result;
    }
}
