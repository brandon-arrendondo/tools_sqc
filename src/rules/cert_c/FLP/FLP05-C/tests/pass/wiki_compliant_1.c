/*
 * Rule: FLP05-C
 * Source: wiki
 * Status: PASS - Compliant solution
 * Description: Using double has more range and handles small values better
 */

#include <stdio.h>

void compliant(void) {
    double x = 1/3.0;
    printf("Original    : %e\n", x);
    x = x * 7e-45;
    printf("Denormalized: %e\n", x);
    x = x / 7e-45;
    printf("Restored    : %e\n", x);
}
