/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Increment operation checks for INT_MAX before incrementing to avoid overflow
 */

#include <limits.h>
#include <stdio.h>

int safe_increment(int *value) {
    if (*value == INT_MAX) {
        return -1; // Increment would overflow
    }
    (*value)++;
    return 0;
}

int main() {
    int value1 = 42;
    int value2 = INT_MAX;

    printf("Initial value1: %d\n", value1);
    if (safe_increment(&value1) == 0) {
        printf("After increment: %d\n", value1);
    } else {
        printf("Increment would overflow\n");
    }

    printf("Initial value2: %d\n", value2);
    if (safe_increment(&value2) == 0) {
        printf("After increment: %d\n", value2);
    } else {
        printf("Increment would overflow\n");
    }

    return 0;
}