/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Factorial calculation can easily overflow for moderate input values
 */

#include <limits.h>
#include <stdio.h>

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    // VIOLATION: no overflow checking in multiplication
    return n * factorial(n - 1);
}

int main() {
    int values[] = {5, 10, 15, 20};
    int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        int result = factorial(values[i]);
        printf("factorial(%d) = %d\n", values[i], result);
    }

    return 0;
}