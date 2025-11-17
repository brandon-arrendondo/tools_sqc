/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: memcmp size exceeds first buffer
 */

#include <string.h>

void memcmp_exceed(void) {
    char buf1[10] = "Short";
    char buf2[50] = "Much longer buffer";

    // Compares 50 bytes but buf1 is only 10 bytes
    int result = memcmp(buf1, buf2, 50);  // Line 14 - VIOLATION
}

int main(void) {
    memcmp_exceed();
    return 0;
}
