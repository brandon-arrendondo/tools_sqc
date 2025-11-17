/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: memset size exceeds calloc'd memory
 */

#include <stdlib.h>
#include <string.h>

void calloc_exceed(void) {
    // Allocate 50 bytes
    char *ptr = (char *)calloc(50, sizeof(char));

    if (ptr) {
        // Try to set 100 bytes
        memset(ptr, 0xFF, 100);  // Line 15 - VIOLATION

        free(ptr);
    }
}

int main(void) {
    calloc_exceed();
    return 0;
}
