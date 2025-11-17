/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: wcsncat with count that overflows destination
 */

#include <wchar.h>

void wcsncat_exceed(void) {
    wchar_t dest[20] = L"Start";
    const wchar_t *src = L" and more text";

    // dest has 20 elements, tries to append 50
    wcsncat(dest, src, 50);  // Line 13 - VIOLATION
}

int main(void) {
    wcsncat_exceed();
    return 0;
}
