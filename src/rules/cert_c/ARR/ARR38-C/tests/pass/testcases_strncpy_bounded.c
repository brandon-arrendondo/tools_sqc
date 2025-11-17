/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: strncpy with size limited to destination buffer
 */

#include <string.h>

void safe_strncpy(void) {
    char dest[20];
    const char *src = "This is a source string";

    // Use destination buffer size - COMPLIANT
    strncpy(dest, src, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';  // Ensure null termination
}

int main(void) {
    safe_strncpy();
    return 0;
}
