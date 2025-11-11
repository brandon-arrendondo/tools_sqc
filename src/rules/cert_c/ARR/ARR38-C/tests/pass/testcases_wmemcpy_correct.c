/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: Using element count (not sizeof) with wmemcpy
 */

#include <string.h>
#include <wchar.h>

static const wchar_t w_str[] = L"Hello world";

void func(void) {
    wchar_t w_buffer[32];

    // Use element count, not bytes - COMPLIANT
    wmemcpy(w_buffer, w_str, wcslen(w_str) + 1);
}

int main(void) {
    func();
    return 0;
}
