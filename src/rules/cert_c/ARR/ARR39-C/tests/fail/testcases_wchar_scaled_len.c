/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Multiplying string length by sizeof(wchar_t) causes double-scaling
 */

#include <wchar.h>
#include <stdio.h>

enum { WCHAR_BUF = 128 };

void func(void) {
    wchar_t error_msg[WCHAR_BUF];

    wcscpy(error_msg, L"Error: ");

    // Manually scaling by sizeof(wchar_t) - double-scaling
    fgetws(error_msg + wcslen(error_msg) * sizeof(wchar_t),  // Line 18 - VIOLATION
           WCHAR_BUF - 7, stdin);
}

int main(void) {
    func();
    return 0;
}
