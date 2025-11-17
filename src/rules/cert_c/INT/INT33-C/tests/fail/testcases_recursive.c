/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Recursive function eventually returns zero used as divisor
 */

#include <stdio.h>

int countdown(int n) {
    if (n <= 0) {
        return 0;  // Base case returns zero
    }
    return countdown(n - 1);
}

int main() {
    int dividend = 84;
    int divisor = countdown(3);  // Eventually returns 0

    int result = dividend / divisor;  // Division by zero
    printf("Result: %d\n", result);
    return 0;
}