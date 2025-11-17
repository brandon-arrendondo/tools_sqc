/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: memcpy size exceeding aligned_alloc'd memory
 */

#include <stdlib.h>
#include <string.h>

void aligned_exceed(void) {
    // Allocate 64 bytes aligned to 16-byte boundary
    char *ptr = (char *)aligned_alloc(16, 64);

    if (ptr) {
        // Try to copy 128 bytes into 64-byte allocation
        char src[128] = {0};
        memcpy(ptr, src, sizeof(src));  // Line 16 - VIOLATION

        free(ptr);
    }
}

int main(void) {
    aligned_exceed();
    return 0;
}
