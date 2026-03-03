/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - No violation expected
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: 1000000 << 10 = 1,024,000,000 which fits in INT_MAX (2,147,483,647).
 *         Const_eval resolves the local assignment and proves this is safe.
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int value = 1000000;
    int result = value << 10; // OK: 1000000 << 10 = 1024000000, fits in int

    printf("Result: %d\n", result);
    return 0;
}
