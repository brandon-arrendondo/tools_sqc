/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Double pointer array access beyond allocated bounds
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int **matrix = malloc(3 * sizeof(int*));

    if (matrix != NULL) {
        for (int i = 0; i < 3; i++) {
            matrix[i] = malloc(4 * sizeof(int));
        }

        // Access beyond matrix bounds
        if (matrix[0] != NULL) {
            matrix[0][5] = 999;  // Beyond column bounds
        }

        // Access beyond row bounds
        if (matrix[5] != NULL) {  // Beyond allocated rows
            matrix[5][0] = 888;
        }

        // Cleanup
        for (int i = 0; i < 3; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }

    return 0;
}