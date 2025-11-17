/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Accesses old pointer after realloc may have freed it
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(5 * sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    for (int i = 0; i < 5; i++) {
        ptr[i] = i;
    }

    int *old_ptr = ptr;
    ptr = realloc(ptr, 10 * sizeof(int));

    if (ptr == NULL) {
        free(old_ptr);
        return -1;
    }

    // BUG: Access old pointer after realloc
    printf("Old value: %d\n", old_ptr[0]);

    free(ptr);
    return 0;
}