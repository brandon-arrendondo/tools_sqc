/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: memchr searching beyond buffer bounds
 */

#include <string.h>
#include <stdio.h>

void memchr_exceed(void) {
    char buffer[15] = "Hello World";

    // Searches 100 bytes in 15-byte buffer
    char *found = memchr(buffer, 'X', 100);  // Line 13 - VIOLATION

    if (found) {
        printf("Found at position: %ld\n", found - buffer);
    }
}

int main(void) {
    memchr_exceed();
    return 0;
}
