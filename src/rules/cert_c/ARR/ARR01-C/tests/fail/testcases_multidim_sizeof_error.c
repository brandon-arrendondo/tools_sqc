/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>

void process_matrix(int matrix[][5]) {
    size_t rows = sizeof(matrix) / sizeof(matrix[0]);

    for (size_t i = 0; i < rows; i++) {
        for (int j = 0; j < 5; j++) {
            matrix[i][j] = i + j;
        }
    }
}

int main() {
    int grid[10][5];

    process_matrix(grid);

    return 0;
}