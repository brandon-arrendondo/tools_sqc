/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Unsafe macro performs division without zero checking
 */

#include <stdio.h>

#define UNSAFE_DIVIDE(a, b) ((a) / (b))  // No zero check in macro

int main() {
    int x = 25, y = 0;
    int result = UNSAFE_DIVIDE(x, y);  // Macro doesn't check for zero
    printf("Result: %d\n", result);
    return 0;
}