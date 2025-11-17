/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(10 * sizeof(int));

    if (ptr == NULL) {
        return 1;
    }

    // Initialize array
    for (int i = 0; i < 10; i++) {
        ptr[i] = i;
    }

    // Misuse of realloc
    int *new_ptr = realloc(ptr, 20 * sizeof(int));

    if (new_ptr) {
        // Realloc succeeded, but we incorrectly free the old pointer
        free(ptr);     // Error: ptr may have been freed by realloc

        // Use new_ptr
        for (int i = 10; i < 20; i++) {
            new_ptr[i] = i;
        }

        free(new_ptr);  // Correct free
    } else {
        // Realloc failed
        free(ptr);      // Correct free of original
    }

    return 0;
}