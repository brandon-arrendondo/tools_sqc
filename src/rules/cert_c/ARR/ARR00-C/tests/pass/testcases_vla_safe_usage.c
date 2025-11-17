/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1000

void process_vla(size_t n) {
    if (n == 0 || n > MAX_SIZE) {
        printf("Invalid array size\n");
        return;
    }

    int vla[n];

    for (size_t i = 0; i < n; i++) {
        vla[i] = i;
    }

    printf("VLA initialized with %zu elements\n", n);
}

void matrix_vla(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0 || rows > 100 || cols > 100) {
        printf("Invalid matrix dimensions\n");
        return;
    }

    int matrix[rows][cols];

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j;
        }
    }

    printf("VLA matrix %zux%zu created\n", rows, cols);
}

int main() {
    process_vla(10);
    process_vla(100);
    matrix_vla(5, 5);

    return 0;
}