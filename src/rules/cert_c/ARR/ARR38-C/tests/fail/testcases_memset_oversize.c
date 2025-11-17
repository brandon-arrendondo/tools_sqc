/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: memset with size exceeding allocated memory
 */

#include <stdlib.h>
#include <string.h>

void oversized_memset(size_t nchars) {
    char *p = (char *)malloc(nchars);

    if (p) {
        const size_t n = nchars + 1;  // Exceeds allocated memory
        memset(p, 0, n);  // Line 15 - VIOLATION

        free(p);
    }
}

int main(void) {
    oversized_memset(100);
    return 0;
}
