/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof with complex number type
 */

#include <complex.h>

void complex_sizeof(void) {
    double complex numbers[40];
    double complex *ptr = numbers;
    int idx = 10;

    // Scaling by sizeof(double complex)
    double complex *target = ptr + (idx * sizeof(double complex));  // Line 14 - VIOLATION
    *target = 1.0 + 2.0 * I;
}

int main(void) {
    complex_sizeof();
    return 0;
}
