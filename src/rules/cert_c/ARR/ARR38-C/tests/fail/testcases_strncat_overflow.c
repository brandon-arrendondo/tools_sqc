/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: strncat with count that could overflow destination
 */

#include <string.h>

void strncat_exceed(void) {
    char dest[15] = "Hello";
    const char *src = " World!";

    // dest has 15 bytes, "Hello" uses 6, but trying to append 20
    strncat(dest, src, 20);  // Line 13 - VIOLATION
}

int main(void) {
    strncat_exceed();
    return 0;
}
