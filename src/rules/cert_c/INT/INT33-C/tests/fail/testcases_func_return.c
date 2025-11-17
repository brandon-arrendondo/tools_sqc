/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Function returns zero which is directly used as divisor
 */

#include <stdio.h>

int get_zero() {
    return 0;  // Function explicitly returns zero
}

int main() {
    int dividend = 60;
    int result = dividend / get_zero();  // Direct use of function return
    printf("Result: %d\n", result);
    return 0;
}