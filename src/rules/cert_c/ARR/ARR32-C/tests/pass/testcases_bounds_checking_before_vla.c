/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>
#include <limits.h>

#define REASONABLE_LIMIT 500

int safe_matrix_operation(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        printf("Error: Matrix dimensions must be positive\n");
        return -1;
    }

    if (rows > REASONABLE_LIMIT || cols > REASONABLE_LIMIT) {
        printf("Error: Matrix dimensions too large\n");
        return -1;
    }

    if (rows > SIZE_MAX / cols) {
        printf("Error: Matrix size would overflow\n");
        return -1;
    }

    int matrix[rows][cols];

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j;
        }
    }

    printf("Created and initialized %zux%zu matrix\n", rows, cols);
    return 0;
}

int main() {
    safe_matrix_operation(10, 20);
    safe_matrix_operation(5, 5);
    safe_matrix_operation(100, 3);

    return 0;
}