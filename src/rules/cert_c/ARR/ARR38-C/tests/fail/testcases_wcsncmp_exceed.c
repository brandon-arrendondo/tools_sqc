/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: wcsncmp with count exceeding buffers
 */

#include <wchar.h>

void wcsncmp_overrun(void) {
    wchar_t str1[12] = L"Hello";
    wchar_t str2[12] = L"Help";

    // Compares 80 elements from 12-element buffers
    int result = wcsncmp(str1, str2, 80);  // Line 13 - VIOLATION
}

int main(void) {
    wcsncmp_overrun();
    return 0;
}
