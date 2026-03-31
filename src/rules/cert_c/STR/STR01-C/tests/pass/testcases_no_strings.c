/*
 * Rule: STR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR01-C violation
 * Description: No string management at all
 */

int add(int a, int b) {
    return a + b;
}

double average(const double *vals, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += vals[i];
    }
    return n > 0 ? sum / n : 0.0;
}
