/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Wrapped multiplication used with realloc
 */

#include <stdlib.h>
#include <stddef.h>

void grow_buffer(void *old_ptr, size_t old_count, size_t growth) {
    // Addition may wrap
    size_t new_count = old_count + growth;  // Line 11 - VIOLATION

    // Multiplication may wrap
    void *new_ptr = realloc(old_ptr, new_count * sizeof(int));  // Line 14 - VIOLATION

    if (new_ptr) {
        free(new_ptr);
    }
}

int main(void) {
    int *ptr = malloc(100 * sizeof(int));
    if (ptr) {
        grow_buffer(ptr, SIZE_MAX / 8, SIZE_MAX / 2);
    }
    return 0;
}
