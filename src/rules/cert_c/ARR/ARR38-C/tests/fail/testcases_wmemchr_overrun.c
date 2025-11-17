/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: wmemchr searching beyond buffer bounds
 */

#include <wchar.h>
#include <stdio.h>

void wmemchr_exceed(void) {
    wchar_t buffer[25] = L"Wide string";

    // Searches 100 wchar_t elements in 25-element buffer
    wchar_t *found = wmemchr(buffer, L'X', 100);  // Line 13 - VIOLATION
}

int main(void) {
    wmemchr_exceed();
    return 0;
}
