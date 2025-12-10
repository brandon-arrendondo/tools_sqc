/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int allocate_and_process(size_t size) {
    int *buffer = malloc(size * sizeof(int));

    if (buffer == NULL) {
        printf("Allocation failed\n");
        return -1;
    }

    // Process data
    for (size_t i = 0; i < size; i++) {
        buffer[i] = i;
    }

    // Bug: Missing return after free in error path
    // This causes a double free when size > 1000
    if (size > 1000) {
        printf("Size too large, cleaning up\n");
        free(buffer);
        // BUG: Missing return - falls through to second free!
    }

    // Normal cleanup - but this is also reached when size > 1000!
    free(buffer);  // DOUBLE FREE when size > 1000

    return 0;
}