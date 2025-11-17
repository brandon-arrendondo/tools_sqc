/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: VLA with memset size exceeding actual allocation
 */

#include <string.h>

void vla_exceed(int n) {
    char vla[n];  // n = 20

    // memset with size larger than VLA
    memset(vla, 0, n + 50);  // Line 12 - VIOLATION
}

int main(void) {
    vla_exceed(20);
    return 0;
}
