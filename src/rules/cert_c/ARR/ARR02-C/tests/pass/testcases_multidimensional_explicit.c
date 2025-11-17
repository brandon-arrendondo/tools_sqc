/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

#define ROWS 4
#define COLS 5

int main() {
    int matrix[ROWS][COLS] = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10}
    };

    char grid[10][10] = {
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'X', 'O', 'X'}
    };

    double tensor[2][3][4] = {
        {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}},
        {{7.0, 8.0}, {9.0, 10.0}}
    };

    int lookup[8][8] = {[0][0] = 1, [7][7] = 64};

    printf("Multidimensional arrays with explicit bounds\n");

    return 0;
}