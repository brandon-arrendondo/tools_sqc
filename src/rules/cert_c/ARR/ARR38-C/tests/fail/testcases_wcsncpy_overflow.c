/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: wcsncpy with count exceeding destination
 */

#include <wchar.h>

void wcsncpy_exceed(void) {
    wchar_t dest[10];
    const wchar_t *src = L"This is a very long wide string";

    // Tries to copy 50 wide characters into 10-element buffer
    wcsncpy(dest, src, 50);  // Line 13 - VIOLATION
}

int main(void) {
    wcsncpy_exceed();
    return 0;
}
