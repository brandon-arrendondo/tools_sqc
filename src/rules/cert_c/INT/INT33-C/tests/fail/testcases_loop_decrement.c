/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Loop variable becomes zero and is used as divisor without checking
 */

#include <stdio.h>

int main() {
    for (int i = 5; i >= 0; i--) {
        int result = 100 / i;  // When i becomes 0, this causes divide by zero
        printf("100 / %d = %d\n", i, result);
    }
    return 0;
}