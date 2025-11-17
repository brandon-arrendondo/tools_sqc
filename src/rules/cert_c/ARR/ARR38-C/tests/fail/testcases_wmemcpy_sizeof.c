/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: Using sizeof() with wmemcpy which expects element count, not bytes
 */

#include <string.h>
#include <wchar.h>

static const wchar_t w_str[] = L"Hello world";

void func(void) {
    wchar_t w_buffer[32];

    // wmemcpy expects element count, not bytes
    wmemcpy(w_buffer, w_str, sizeof(w_str));  // Line 15 - VIOLATION
}

int main(void) {
    func();
    return 0;
}
