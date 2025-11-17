/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Pointer is reassigned, losing reference to original memory
 */

#include <stdlib.h>

void reassign_pointer() {
    char *ptr = malloc(100);
    if (ptr == NULL) {
        return;
    }

    // Fill with data
    for (int i = 0; i < 100; i++) {
        ptr[i] = 'X';
    }

    // Reassign pointer, losing reference to original memory
    ptr = malloc(200);  // Original 100 bytes are now leaked

    if (ptr != NULL) {
        free(ptr);  // Only frees the second allocation
    }
}