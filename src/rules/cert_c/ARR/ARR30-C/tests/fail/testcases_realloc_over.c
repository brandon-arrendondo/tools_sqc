/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Using old pointer size after realloc to smaller size
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr = malloc(10 * sizeof(int));

    if (ptr != NULL) {
        for (int i = 0; i < 10; i++) {
            ptr[i] = i;
        }

        // Shrink allocation but still access old range
        ptr = realloc(ptr, 5 * sizeof(int));

        if (ptr != NULL) {
            // Access beyond new smaller size
            printf("ptr[8] = %d\n", ptr[8]);
            ptr[9] = 999;

            free(ptr);
        }
    }

    return 0;
}