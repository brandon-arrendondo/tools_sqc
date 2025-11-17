/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Accesses memory after it has been freed
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 42;
    free(ptr);

    // BUG: Use-after-free
    printf("Value: %d\n", *ptr);

    return 0;
}