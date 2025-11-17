/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Manually multiplying index by sizeof(int)
 */

void manual_scale(void) {
    int array[100];
    int *ptr = array;
    int index = 10;

    // Manually scaling index by sizeof(int)
    int *target = ptr + (index * sizeof(int));  // Line 13 - VIOLATION
    *target = 42;
}

int main(void) {
    manual_scale();
    return 0;
}
