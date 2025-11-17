/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Frees through one pointer but accesses through alias
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    int *alias = ptr;  // Create alias
    *ptr = 88;

    free(ptr);

    // BUG: Access through alias after original freed
    printf("Value via alias: %d\n", *alias);

    return 0;
}