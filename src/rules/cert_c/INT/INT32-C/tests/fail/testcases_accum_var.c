/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Accumulator variable can overflow during repeated additions
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int accumulator = 0;
    int increment = 100000;

    // VIOLATION: no overflow check in accumulation loop
    for (int i = 0; i < 25000; i++) {
        accumulator += increment;
        if (i % 5000 == 0) {
            printf("Step %d: accumulator = %d\n", i, accumulator);
        }
    }

    printf("Final accumulator: %d\n", accumulator);
    return 0;
}