/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Goto jumps past free, but later code accesses freed memory
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 77;
    int error = 0;

    if (error) {
        goto cleanup;
    }

    printf("Initial value: %d\n", *ptr);

cleanup:
    free(ptr);

    // BUG: Access after cleanup
    printf("Final value: %d\n", *ptr);

    return 0;
}