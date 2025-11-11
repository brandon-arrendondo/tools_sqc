/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Union field containing zero used as divisor without validation
 */

#include <stdio.h>

union Value {
    int integer;
    float floating;
};

int main() {
    union Value val;
    val.integer = 0;  // Union field set to zero

    int dividend = 28;
    int result = dividend / val.integer;  // No validation of union field
    printf("Result: %d\n", result);
    return 0;
}