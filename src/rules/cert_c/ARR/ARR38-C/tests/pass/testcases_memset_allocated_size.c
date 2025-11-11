/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: memset with size matching allocated memory
 */

#include <stdlib.h>
#include <string.h>

void correct_memset(size_t nchars) {
    char *p = (char *)malloc(nchars);

    if (p) {
        // Use allocated size - COMPLIANT
        const size_t n = nchars;
        memset(p, 0, n);

        free(p);
    }
}

int main(void) {
    correct_memset(100);
    return 0;
}
