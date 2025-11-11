/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: memcpy with offsetted pointer and original size
 */

#include <string.h>

void offset_memcpy(void) {
    char buffer[30];
    char src[20] = "Source data";

    // Pointer offset by 15, then copy 20 bytes - exceeds buffer
    char *dest_ptr = buffer + 15;
    memcpy(dest_ptr, src, sizeof(src));  // Line 14 - VIOLATION (goes past end)
}

int main(void) {
    offset_memcpy();
    return 0;
}
