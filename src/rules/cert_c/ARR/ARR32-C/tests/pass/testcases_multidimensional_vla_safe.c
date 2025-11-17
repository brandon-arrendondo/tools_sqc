/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>

#define MAX_DIMENSION 100

int safe_3d_array_processing(size_t x, size_t y, size_t z) {
    // Validate all dimensions
    if (x == 0 || y == 0 || z == 0) {
        printf("Error: All dimensions must be positive\n");
        return -1;
    }

    if (x > MAX_DIMENSION || y > MAX_DIMENSION || z > MAX_DIMENSION) {
        printf("Error: Dimensions exceed maximum allowed size\n");
        return -1;
    }

    // Check for potential overflow
    if (x > SIZE_MAX / y || (x * y) > SIZE_MAX / z) {
        printf("Error: Total array size would overflow\n");
        return -1;
    }

    size_t total_size = x * y * z * sizeof(int);
    if (total_size > 16384) {  // 16KB limit
        printf("Error: 3D array too large for stack allocation\n");
        return -1;
    }

    int array_3d[x][y][z];

    // Initialize the 3D array
    for (size_t i = 0; i < x; i++) {
        for (size_t j = 0; j < y; j++) {
            for (size_t k = 0; k < z; k++) {
                array_3d[i][j][k] = i + j + k;
            }
        }
    }

    printf("Successfully created and initialized %zux%zux%zu 3D VLA\n", x, y, z);
    return 0;
}

void safe_2d_matrix(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0 || rows > MAX_DIMENSION || cols > MAX_DIMENSION) {
        printf("Invalid matrix dimensions: %zux%zu\n", rows, cols);
        return;
    }

    float matrix[rows][cols];

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix[i][j] = (float)(i * cols + j);
        }
    }

    printf("Created %zux%zu matrix VLA\n", rows, cols);
}

int main() {
    // Safe multidimensional VLA usage
    safe_2d_matrix(5, 10);
    safe_2d_matrix(20, 30);

    safe_3d_array_processing(4, 5, 6);
    safe_3d_array_processing(10, 8, 4);

    return 0;
}