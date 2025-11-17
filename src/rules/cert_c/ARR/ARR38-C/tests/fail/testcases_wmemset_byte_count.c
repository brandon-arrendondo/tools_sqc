/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: wmemset expects element count but given byte count
 */

#include <wchar.h>

void wmemset_wrong_count(void) {
    wchar_t buffer[50];

    // wmemset expects wchar_t count, not byte count
    wmemset(buffer, L'X', sizeof(buffer));  // Line 13 - VIOLATION
}

int main(void) {
    wmemset_wrong_count();
    return 0;
}
