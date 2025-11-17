/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>

#define ROWS 4
#define COLS 5

void process_matrix(int matrix[][COLS], size_t rows) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < COLS; j++) {
            matrix[i][j] = i * COLS + j;
        }
    }
}

void print_matrix(int matrix[][COLS], size_t rows) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < COLS; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int grid[ROWS][COLS];

    process_matrix(grid, ROWS);
    print_matrix(grid, ROWS);

    size_t total_elements = ROWS * COLS;
    size_t matrix_bytes = sizeof(grid);

    printf("Matrix: %dx%d = %zu elements\n", ROWS, COLS, total_elements);
    printf("Total size: %zu bytes\n", matrix_bytes);

    return 0;
}