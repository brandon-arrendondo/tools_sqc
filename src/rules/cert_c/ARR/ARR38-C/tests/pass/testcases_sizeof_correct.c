/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: Using sizeof on array directly for correct size
 */

#include <string.h>

void proper_sizeof(void) {
    const size_t ARR_SIZE = 4;
    long a[ARR_SIZE];

    // Direct sizeof - no manual scaling - COMPLIANT
    const size_t n = sizeof(a);
    void *p = a;
    memset(p, 0, n);
}

int main(void) {
    proper_sizeof();
    return 0;
}
