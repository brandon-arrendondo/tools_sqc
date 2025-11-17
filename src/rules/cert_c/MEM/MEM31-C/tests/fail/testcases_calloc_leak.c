/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Memory allocated with calloc is never freed
 */

#include <stdlib.h>

void matrix_operation() {
    double *matrix = calloc(64, sizeof(double));
    if (matrix == NULL) {
        return;
    }

    // Initialize matrix
    for (int i = 0; i < 64; i++) {
        matrix[i] = i * 2.5;
    }

    // Perform calculations
    double sum = 0.0;
    for (int i = 0; i < 64; i++) {
        sum += matrix[i];
    }

    printf("Matrix sum: %f\n", sum);

    // Memory allocated with calloc is never freed - MEMORY LEAK
}