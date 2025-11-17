/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Conditional free but unconditional access afterwards
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 25;
    int condition = 1;

    if (condition) {
        free(ptr);
    }

    // BUG: Always accesses, but may be freed
    printf("Value: %d\n", *ptr);

    return 0;
}