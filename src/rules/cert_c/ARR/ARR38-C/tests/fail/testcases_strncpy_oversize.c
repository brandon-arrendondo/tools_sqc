/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: strncpy with size exceeding destination buffer
 */

#include <string.h>

void strncpy_exceed(void) {
    char dest[10];
    const char *src = "This is a very long string that will overflow";

    // Tries to copy 50 bytes into 10-byte buffer
    strncpy(dest, src, 50);  // Line 13 - VIOLATION
}

int main(void) {
    strncpy_exceed();
    return 0;
}
