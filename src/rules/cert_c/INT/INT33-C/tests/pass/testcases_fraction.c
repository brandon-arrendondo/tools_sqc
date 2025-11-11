/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: Fraction structure validates denominator is non-zero before creation
 */

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int numerator;
    int denominator;
} Fraction;

bool create_fraction(Fraction *frac, int num, int denom) {
    if (denom == 0) {
        printf("Error: Denominator cannot be zero\n");
        return false;
    }
    frac->numerator = num;
    frac->denominator = denom;
    return true;
}

double fraction_to_decimal(Fraction *frac) {
    return (double)frac->numerator / frac->denominator;
}

int main() {
    Fraction f;
    if (create_fraction(&f, 3, 4)) {
        printf("Fraction: %d/%d = %.2f\n", f.numerator, f.denominator, fraction_to_decimal(&f));
    }
    return 0;
}