/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Array elements are allocated but not all are freed
 */

#include <stdlib.h>

void allocate_array_elements() {
    char *array[5];

    // Allocate memory for each element
    for (int i = 0; i < 5; i++) {
        array[i] = malloc(50);
        if (array[i] != NULL) {
            array[i][0] = 'A' + i;
        }
    }

    // Only free some elements
    for (int i = 0; i < 3; i++) {
        free(array[i]);
    }

    // array[3] and array[4] are never freed - MEMORY LEAK
}