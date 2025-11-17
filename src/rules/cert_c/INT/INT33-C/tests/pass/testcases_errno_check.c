/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: Division with errno handling and proper error checking mechanisms
 */

#include <stdio.h>
#include <errno.h>
#include <limits.h>

int safe_divide_with_errno(int dividend, int divisor) {
    errno = 0;  // Clear errno before operation

    if (divisor == 0) {
        errno = EDOM;  // Domain error
        printf("Error: Division by zero (errno: %d)\n", errno);
        return 0;
    }

    // Check for overflow in division
    if (dividend == INT_MIN && divisor == -1) {
        errno = ERANGE;  // Range error
        printf("Error: Division overflow (errno: %d)\n", errno);
        return 0;
    }

    return dividend / divisor;
}

int main() {
    int result1 = safe_divide_with_errno(10, 2);
    printf("10 / 2 = %d (errno: %d)\n", result1, errno);

    int result2 = safe_divide_with_errno(10, 0);
    printf("Division result: %d (errno: %d)\n", result2, errno);

    return 0;
}