/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Wrapped addition for buffer copy size
 */

#include <string.h>
#include <stddef.h>

void copy_buffers(char *dest, const char *src, size_t size1, size_t size2) {
    // Addition may wrap
    size_t total_size = size1 + size2;  // Line 11 - VIOLATION

    memcpy(dest, src, total_size);
}

int main(void) {
    char dest[100], src[100];
    copy_buffers(dest, src, SIZE_MAX - 50, 100);  // Will wrap
    return 0;
}
