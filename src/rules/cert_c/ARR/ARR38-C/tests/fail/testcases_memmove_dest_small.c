/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: memmove size exceeds destination buffer
 */

#include <string.h>

void memmove_exceed(void) {
    char src[100] = "Source data";
    char dest[20];

    // Moves 100 bytes into 20-byte buffer
    memmove(dest, src, sizeof(src));  // Line 14 - VIOLATION
}

int main(void) {
    memmove_exceed();
    return 0;
}
