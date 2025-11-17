/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Dynamic array allocation includes proper size tracking
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t count = 6;
    int *dynamic_arr = malloc(count * sizeof(int));

    if (dynamic_arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Initialize with bounds checking
    for (size_t i = 0; i < count; i++) {
        dynamic_arr[i] = i * i;
    }

    // Access with bounds checking
    for (size_t i = 0; i < count; i++) {
        printf("dynamic_arr[%zu] = %d\n", i, dynamic_arr[i]);
    }

    free(dynamic_arr);
    return 0;
}