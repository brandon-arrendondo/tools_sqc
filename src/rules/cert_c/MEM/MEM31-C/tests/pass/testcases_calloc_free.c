/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: Memory allocated with calloc is properly freed after use
 */

#include <stdlib.h>
#include <stdio.h>

void matrix_operation() {
    // Allocate zero-initialized memory
    double *matrix = calloc(100, sizeof(double));
    if (matrix == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    // Perform operations on the matrix
    for (int i = 0; i < 100; i++) {
        matrix[i] = i * 3.14;
    }

    // Properly free the allocated memory
    free(matrix);
}