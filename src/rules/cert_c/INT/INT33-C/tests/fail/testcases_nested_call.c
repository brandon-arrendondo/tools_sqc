/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Nested function call returns zero which is used as divisor
 */

#include <stdio.h>

int get_divisor(int x) {
    return x - 5;  // Returns 0 when x is 5
}

int calculate(int dividend, int x) {
    return dividend / get_divisor(x);  // No check on return value
}

int main() {
    int result = calculate(20, 5);  // get_divisor(5) returns 0
    printf("Result: %d\n", result);
    return 0;
}