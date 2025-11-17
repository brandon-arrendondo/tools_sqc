/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single complex number variable
 */

#include <complex.h>

void complex_test(void) {
    double complex z = 1.0 + 2.0 * I;
    double complex *ptr = &z;

    // Pointer arithmetic on single complex variable
    ptr++;  // Line 14 - VIOLATION
    *ptr = 3.0 + 4.0 * I;  // Undefined behavior
}

int main(void) {
    complex_test();
    return 0;
}
