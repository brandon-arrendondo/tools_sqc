/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Conditional logic ensures memory is only freed once and not accessed after
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 50;
    int should_free = 1;

    if (should_free) {
        printf("Value before free: %d\n", *ptr);
        free(ptr);
        ptr = NULL;
        should_free = 0;  // Mark as freed
    }

    // Safe check before any potential access
    if (ptr != NULL) {
        printf("Value: %d\n", *ptr);
    } else {
        printf("Pointer is NULL\n");
    }

    return 0;
}