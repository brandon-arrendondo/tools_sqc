/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: loop_limits.c
 *
 * This case demonstrates violations where loop limits and bounds
 * that never change are not const-qualified.
 */

#include <stdio.h>

void process_matrix(void) {
    /* NON-COMPLIANT: Matrix dimensions should be const */
    int rows = 3;
    int cols = 4;
    
    /* NON-COMPLIANT: Matrix data that won't be modified */
    int matrix[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    
    printf("Matrix (%dx%d):\n", rows, cols);
    
    /* Loop limits are never modified */
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void iterate_with_bounds(void) {
    /* NON-COMPLIANT: Loop bounds should be const */
    int start_index = 10;
    int end_index = 20;
    int step_size = 2;
    
    printf("\nIterating from %d to %d with step %d:\n", 
           start_index, end_index, step_size);
    
    /* Bounds are never modified during iteration */
    for (int i = start_index; i <= end_index; i += step_size) {
        printf("%d ", i);
    }
    printf("\n");
}

void nested_loop_processing(void) {
    /* NON-COMPLIANT: Nested loop limits should be const */
    int outer_limit = 3;
    int inner_limit = 5;
    int depth_limit = 2;
    
    printf("\nNested loop processing:\n");
    
    /* All limits remain constant */
    for (int depth = 0; depth < depth_limit; depth++) {
        printf("Depth %d:\n", depth);
        for (int i = 0; i < outer_limit; i++) {
            for (int j = 0; j < inner_limit; j++) {
                printf("  (%d,%d,%d) ", depth, i, j);
            }
            printf("\n");
        }
    }
}

void array_processing(void) {
    /* NON-COMPLIANT: Array size should be const */
    int array_size = 10;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    /* NON-COMPLIANT: Processing parameters should be const */
    int batch_size = 3;
    int num_iterations = 5;
    
    printf("\nArray processing (size=%d):\n", array_size);
    
    /* Parameters never change during processing */
    for (int iter = 0; iter < num_iterations && iter < array_size; iter++) {
        printf("Iteration %d: ", iter);
        for (int i = 0; i < batch_size && (iter * batch_size + i) < array_size; i++) {
            int index = iter * batch_size + i;
            if (index < array_size) {
                printf("%d ", data[index]);
            }
        }
        printf("\n");
    }
}

int main(void) {
    /* NON-COMPLIANT: Main loop control should be const */
    int num_tests = 4;
    
    printf("Running %d test functions:\n", num_tests);
    
    for (int test = 0; test < num_tests; test++) {
        printf("\n--- Test %d ---\n", test + 1);
        switch (test) {
            case 0: process_matrix(); break;
            case 1: iterate_with_bounds(); break;
            case 2: nested_loop_processing(); break;
            case 3: array_processing(); break;
        }
    }
    
    return 0;
}