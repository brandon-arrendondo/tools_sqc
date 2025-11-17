/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: User input multiplication without wrap check
 */

#include <stdlib.h>

void allocate_user_buffer(unsigned int user_count) {
    // User-controlled multiplication - security risk
    size_t size = user_count * 1024;  // Line 10 - VIOLATION

    void *buffer = malloc(size);
    if (buffer) {
        free(buffer);
    }
}

int main(void) {
    // Simulate malicious user input
    allocate_user_buffer(UINT_MAX / 512);  // Will wrap
    return 0;
}
