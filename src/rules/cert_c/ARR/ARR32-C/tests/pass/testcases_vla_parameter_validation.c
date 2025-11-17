/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>

#define MAX_ARRAY_SIZE 1000

void process_vla_parameter(size_t n, int array[n]) {
    // VLA parameter - size already validated by caller
    for (size_t i = 0; i < n; i++) {
        array[i] = array[i] * 2;
    }
    printf("Processed VLA parameter of size %zu\n", n);
}

int safe_function_with_vla_param(size_t size) {
    if (size == 0 || size > MAX_ARRAY_SIZE) {
        printf("Invalid size for VLA parameter: %zu\n", size);
        return -1;
    }

    int local_array[size];

    // Initialize array
    for (size_t i = 0; i < size; i++) {
        local_array[i] = i + 1;
    }

    // Pass to function expecting VLA parameter
    process_vla_parameter(size, local_array);

    return 0;
}

void matrix_operation(size_t rows, size_t cols,
                     double matrix[rows][cols]) {
    // Safe VLA parameter usage
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix[i][j] = matrix[i][j] + 1.0;
        }
    }
    printf("Processed %zux%zu matrix parameter\n", rows, cols);
}

int create_and_process_matrix(size_t r, size_t c) {
    if (r == 0 || c == 0 || r > 50 || c > 50) {
        printf("Invalid matrix dimensions: %zux%zu\n", r, c);
        return -1;
    }

    double local_matrix[r][c];

    // Initialize
    for (size_t i = 0; i < r; i++) {
        for (size_t j = 0; j < c; j++) {
            local_matrix[i][j] = i * c + j;
        }
    }

    // Process with VLA parameter function
    matrix_operation(r, c, local_matrix);

    return 0;
}

int main() {
    // Safe VLA parameter usage
    safe_function_with_vla_param(10);
    safe_function_with_vla_param(100);
    safe_function_with_vla_param(500);

    // Safe matrix operations
    create_and_process_matrix(5, 8);
    create_and_process_matrix(10, 15);
    create_and_process_matrix(20, 25);

    return 0;
}