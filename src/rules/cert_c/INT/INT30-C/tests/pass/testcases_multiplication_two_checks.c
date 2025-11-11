/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Multiple multiplications with checks at each step
 */

#include <stdlib.h>
#include <stddef.h>

void allocate_3d(size_t x, size_t y, size_t z) {
    size_t temp, size;

    // Check first multiplication - COMPLIANT
    if (x > SIZE_MAX / y) {
        return;
    }
    temp = x * y;

    // Check second multiplication - COMPLIANT
    if (temp > SIZE_MAX / z) {
        return;
    }
    size = temp * z;

    // Check for sizeof multiplication - COMPLIANT
    if (size > SIZE_MAX / sizeof(int)) {
        return;
    }

    int *array = malloc(size * sizeof(int));
    if (array) {
        free(array);
    }
}

int main(void) {
    allocate_3d(100, 100, 100);
    return 0;
}
