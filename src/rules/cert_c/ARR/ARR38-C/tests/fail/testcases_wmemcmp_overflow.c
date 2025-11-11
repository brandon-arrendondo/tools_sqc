/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: wmemcmp count exceeds buffer sizes
 */

#include <wchar.h>

void wmemcmp_exceed(void) {
    wchar_t buf1[15];
    wchar_t buf2[15];

    // Compares 100 wchar_t elements from 15-element buffers
    int result = wmemcmp(buf1, buf2, 100);  // Line 13 - VIOLATION
}

int main(void) {
    wmemcmp_exceed();
    return 0;
}
