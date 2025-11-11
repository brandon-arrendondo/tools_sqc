/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: snprintf with size exceeding buffer capacity
 */

#include <stdio.h>

void snprintf_exceed(void) {
    char buffer[20];

    // Claims buffer can hold 100 bytes but it's only 20
    snprintf(buffer, 100, "This is a test string");  // Line 12 - VIOLATION
}

int main(void) {
    snprintf_exceed();
    return 0;
}
