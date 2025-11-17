/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Macro hides use-after-free by expanding to freed pointer access
 */

#include <stdlib.h>
#include <stdio.h>

#define USE_PTR(p) printf("Value: %d\n", *(p))
#define FREE_PTR(p) free(p)

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 777;
    USE_PTR(ptr);

    FREE_PTR(ptr);

    // BUG: Macro expands to use-after-free
    USE_PTR(ptr);

    return 0;
}