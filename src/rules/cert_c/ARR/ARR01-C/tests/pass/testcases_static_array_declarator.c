/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>

void process_fixed_array(int arr[static 5]) {
    for (int i = 0; i < 5; i++) {
        arr[i] += 10;
    }
}

void process_matrix(int matrix[static 3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] *= 2;
        }
    }
}

int main() {
    int numbers[5] = {1, 2, 3, 4, 5};
    int grid[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    process_fixed_array(numbers);
    process_matrix(grid);

    return 0;
}