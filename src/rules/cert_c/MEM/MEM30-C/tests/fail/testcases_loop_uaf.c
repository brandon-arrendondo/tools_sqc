/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Frees memory in loop but continues to use it
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 10;

    for (int i = 0; i < 3; i++) {
        if (i == 1) {
            free(ptr);
        }
        // BUG: Continues to access after free
        printf("Iteration %d: %d\n", i, *ptr);
        (*ptr)++;
    }

    return 0;
}