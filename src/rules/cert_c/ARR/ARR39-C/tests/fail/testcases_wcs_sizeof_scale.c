/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Scaling wide string position by sizeof(wchar_t)
 */

#include <wchar.h>

void wcs_scale(void) {
    wchar_t buffer[100];
    wchar_t *pos;
    size_t len = 10;

    // Manually scaling position by sizeof(wchar_t)
    pos = buffer + (len * sizeof(wchar_t));  // Line 14 - VIOLATION
    *pos = L'X';
}

int main(void) {
    wcs_scale();
    return 0;
}
