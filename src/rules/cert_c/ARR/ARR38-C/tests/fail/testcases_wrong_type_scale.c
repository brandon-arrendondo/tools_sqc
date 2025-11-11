/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: Incorrect size scaling using wrong type size
 */

#include <string.h>

void wrong_scaling(void) {
    const size_t ARR_SIZE = 4;
    long a[ARR_SIZE];

    // Wrong: scales by int not long
    const size_t n = sizeof(int) * ARR_SIZE;  // Line 14 - VIOLATION
    void *p = a;
    memset(p, 0, n);  // Doesn't zero entire array
}

int main(void) {
    wrong_scaling();
    return 0;
}
