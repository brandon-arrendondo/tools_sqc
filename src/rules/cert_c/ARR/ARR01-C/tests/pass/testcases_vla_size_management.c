/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>
#include <stdlib.h>

void process_vla(size_t rows, size_t cols, int matrix[rows][cols]) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix[i][j] = i + j;
        }
    }
}

void print_vla(size_t rows, size_t cols, int matrix[rows][cols]) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    size_t r = 3, c = 4;

    if (r > 0 && c > 0 && r <= 100 && c <= 100) {
        int vla[r][c];

        process_vla(r, c, vla);
        print_vla(r, c, vla);

        printf("VLA dimensions: %zux%zu\n", r, c);
    }

    return 0;
}