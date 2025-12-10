/*
 * Rule: FLP05-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP05-C violation
 * Description: Using float with denormalized value 7e-45 causes precision loss
 */

#include <stdio.h>

void noncompliant(void) {
    float x = 1/3.0;
    printf("Original    : %e\n", x);
    x = x * 7e-45;  /* Violation: multiplying float by denormalized constant */
    printf("Denormalized: %e\n", x);
    x = x / 7e-45;  /* Violation: dividing float by denormalized constant */
    printf("Restored    : %e\n", x);
}
