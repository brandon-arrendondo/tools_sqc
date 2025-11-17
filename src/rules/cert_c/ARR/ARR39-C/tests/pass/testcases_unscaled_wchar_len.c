/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using unscaled string length for pointer arithmetic
 */

#include <wchar.h>
#include <stdio.h>

enum { WCHAR_BUF = 128 };
const wchar_t ERROR_PREFIX[7] = L"Error: ";

void func(void) {
    const size_t prefix_len = wcslen(ERROR_PREFIX);
    wchar_t error_msg[WCHAR_BUF];

    wcscpy(error_msg, ERROR_PREFIX);

    // Use unscaled element count - COMPLIANT
    fgetws(error_msg + prefix_len, WCHAR_BUF - prefix_len, stdin);
}

int main(void) {
    func();
    return 0;
}
