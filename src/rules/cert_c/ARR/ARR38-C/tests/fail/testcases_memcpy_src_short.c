/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: memcpy size exceeds source buffer size
 */

#include <string.h>

void src_too_short(void) {
    char p[40];
    const char *q = "Too short";  // Only 10 bytes including null

    // Tries to copy 40 bytes from 10-byte string
    size_t n = sizeof(p);
    memcpy(p, q, n);  // Line 15 - VIOLATION
}

int main(void) {
    src_too_short();
    return 0;
}
