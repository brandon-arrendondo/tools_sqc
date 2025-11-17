/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: memcpy with proper size validation
 */

#include <string.h>
#include <stdio.h>

void safe_memcpy(void) {
    char dest[40];
    const char *src = "Source string";

    // Use minimum of dest size and actual src length - COMPLIANT
    size_t n = sizeof(dest) < strlen(src) + 1 ? sizeof(dest) : strlen(src) + 1;
    memcpy(dest, src, n);

    printf("Copied: %s\n", dest);
}

int main(void) {
    safe_memcpy();
    return 0;
}
