/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: snprintf with correct buffer size
 */

#include <stdio.h>

void safe_snprintf(void) {
    char buffer[50];

    // Use actual buffer size - COMPLIANT
    snprintf(buffer, sizeof(buffer), "Formatted string: %d", 42);

    printf("%s\n", buffer);
}

int main(void) {
    safe_snprintf();
    return 0;
}
