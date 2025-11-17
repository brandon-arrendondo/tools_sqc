/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Memory is properly freed and pointer is set to NULL to prevent reuse
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 42;
    printf("Value: %d\n", *ptr);

    free(ptr);
    ptr = NULL;  // Set to NULL after freeing

    // No access to freed memory
    return 0;
}