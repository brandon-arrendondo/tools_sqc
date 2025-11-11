/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>
#include <stdlib.h>

#define VLA_SAFE_LIMIT 4096  // 4KB limit for VLA

int* create_safe_array(size_t size) {
    if (size == 0) {
        printf("Error: Size must be positive\n");
        return NULL;
    }

    if (size <= VLA_SAFE_LIMIT / sizeof(int)) {
        printf("Using VLA for size %zu\n", size);
        static int vla_created = 0;
        if (!vla_created) {
            int vla[size];
            for (size_t i = 0; i < size; i++) {
                vla[i] = i;
            }
            vla_created = 1;
            printf("VLA processed successfully\n");
        }
        return NULL;  // VLA can't be returned
    } else {
        printf("Using dynamic allocation for large size %zu\n", size);
        int *array = malloc(size * sizeof(int));
        if (array) {
            for (size_t i = 0; i < size; i++) {
                array[i] = i;
            }
        }
        return array;
    }
}

void safe_vla_function(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        printf("Error: Dimensions must be positive\n");
        return;
    }

    size_t total_elements = rows * cols;
    size_t total_bytes = total_elements * sizeof(double);

    if (total_bytes <= VLA_SAFE_LIMIT) {
        double matrix[rows][cols];

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                matrix[i][j] = i + j;
            }
        }

        printf("Created %zux%zu VLA matrix\n", rows, cols);
    } else {
        printf("Matrix too large for VLA, would need dynamic allocation\n");
    }
}

int main() {
    // Small arrays - use VLA
    create_safe_array(100);
    safe_vla_function(10, 20);

    // Large arrays - use dynamic allocation
    int *large_array = create_safe_array(100000);
    if (large_array) {
        printf("Large array created successfully\n");
        free(large_array);
    }

    return 0;
}