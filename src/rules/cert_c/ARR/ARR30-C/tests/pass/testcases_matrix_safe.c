/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: 2D array access validates both row and column bounds
 */

#include <stdio.h>
#define ROWS 3
#define COLS 4

int main(void) {
    int matrix[ROWS][COLS] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };

    int row, col;
    printf("Enter row (0-%d) and col (0-%d): ", ROWS-1, COLS-1);
    scanf("%d %d", &row, &col);

    // Validate both dimensions before access
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
        printf("matrix[%d][%d] = %d\n", row, col, matrix[row][col]);
    } else {
        printf("Invalid matrix coordinates\n");
    }

    return 0;
}