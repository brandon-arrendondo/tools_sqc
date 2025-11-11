/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Structure field with zero value used as divisor without validation
 */

#include <stdio.h>

typedef struct {
    int numerator;
    int denominator;
} Fraction;

int main() {
    Fraction f = {10, 0};  // Denominator is zero
    double result = (double)f.numerator / f.denominator;  // No validation
    printf("Result: %.2f\n", result);
    return 0;
}