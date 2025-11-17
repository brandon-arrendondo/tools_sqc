/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: Modulo operation with proper zero check to prevent divide-by-zero
 */

#include <stdio.h>

int safe_modulo(int dividend, int divisor) {
    if (divisor == 0) {
        printf("Error: Modulo by zero\n");
        return 0;  // Safe default value
    }
    return dividend % divisor;
}

int main() {
    int x = 15, y = 4;
    int remainder = safe_modulo(x, y);
    printf("Remainder: %d\n", remainder);
    return 0;
}