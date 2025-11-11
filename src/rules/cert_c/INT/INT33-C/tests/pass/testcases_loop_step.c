/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: Loop increment validation prevents zero step causing infinite loop or division by zero
 */

#include <stdio.h>

void process_range(int start, int end, int step) {
    if (step == 0) {
        printf("Error: Step cannot be zero\n");
        return;
    }

    printf("Processing range from %d to %d with step %d:\n", start, end, step);

    if (step > 0) {
        for (int i = start; i <= end; i += step) {
            printf("Value: %d, Position: %d\n", i, (i - start) / step);
        }
    } else {
        for (int i = start; i >= end; i += step) {
            printf("Value: %d, Position: %d\n", i, (start - i) / (-step));
        }
    }
}

int main() {
    process_range(0, 10, 2);
    return 0;
}