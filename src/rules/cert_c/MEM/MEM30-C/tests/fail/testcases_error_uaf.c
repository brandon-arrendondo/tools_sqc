/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Error handling path frees memory but execution continues
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 666;
    int error_occurred = 1;

    if (error_occurred) {
        printf("Error occurred, cleaning up\n");
        free(ptr);
        // BUG: Should return here but doesn't
    }

    // BUG: This code runs even after error cleanup
    printf("Normal operation: %d\n", *ptr);

    return 0;
}