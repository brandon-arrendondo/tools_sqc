/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Multi-dimensional array access beyond bounds
 */

#include <stdio.h>

int main(void) {
    int matrix[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };

    // Access beyond row bounds
    printf("matrix[3][0] = %d\n", matrix[3][0]);

    // Access beyond column bounds
    printf("matrix[1][4] = %d\n", matrix[1][4]);

    // Write beyond bounds
    matrix[3][4] = 999;

    return 0;
}