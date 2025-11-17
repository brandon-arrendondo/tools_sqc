/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Guards against double-free with NULL check, safe pattern
 */

#include <stdlib.h>
#include <stdio.h>

void safe_free(void **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;  // Set to NULL to prevent double-free
    }
}

int main() {
    int *data = malloc(sizeof(int));
    if (data == NULL) {
        return -1;
    }

    *data = 75;
    printf("Value: %d\n", *data);

    // Safe free function
    safe_free((void**)&data);

    // Attempting to free again is safe
    safe_free((void**)&data);

    return 0;
}