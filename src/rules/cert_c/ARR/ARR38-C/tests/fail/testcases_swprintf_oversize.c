/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: swprintf with size exceeding buffer
 */

#include <wchar.h>

void swprintf_exceed(void) {
    wchar_t buffer[30];

    // Claims buffer can hold 100 wide chars but it's only 30
    swprintf(buffer, 100, L"Test string %d", 42);  // Line 12 - VIOLATION
}

int main(void) {
    swprintf_exceed();
    return 0;
}
