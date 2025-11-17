/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof on multidimensional array row
 */

void multidim_sizeof(void) {
    int matrix[5][10];
    int *ptr = &matrix[0][0];
    int row_num = 2;

    // sizeof(matrix[0]) returns bytes for one row
    int *row_start = ptr + (row_num * sizeof(matrix[0]));  // Line 13 - VIOLATION
    *row_start = 77;
}

int main(void) {
    multidim_sizeof();
    return 0;
}
