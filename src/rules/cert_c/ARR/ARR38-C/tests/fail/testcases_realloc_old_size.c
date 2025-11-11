/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: Using old size after realloc to smaller size
 */

#include <stdlib.h>
#include <string.h>

void realloc_size_mismatch(void) {
    char *ptr = (char *)malloc(100);

    if (ptr) {
        // Realloc to smaller size
        char *new_ptr = (char *)realloc(ptr, 50);

        if (new_ptr) {
            // Still using old size - exceeds new allocation
            memset(new_ptr, 0, 100);  // Line 18 - VIOLATION

            free(new_ptr);
        }
    }
}

int main(void) {
    realloc_size_mismatch();
    return 0;
}
