/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: GCD algorithm with proper handling of zero values and modulo operations
 */

#include <stdio.h>

int gcd(int a, int b) {
    if (a == 0 && b == 0) {
        printf("Error: GCD of (0,0) is undefined\n");
        return 0;
    }
    if (a == 0) return b;
    if (b == 0) return a;

    while (b != 0) {
        int temp = b;
        b = a % b;  // Safe because b is checked to be non-zero
        a = temp;
    }
    return a;
}

int main() {
    int x = 48, y = 18;
    int result = gcd(x, y);
    if (result > 0) {
        printf("GCD of %d and %d is %d\n", x, y, result);
    }
    return 0;
}