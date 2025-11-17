/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using direct index without manual scaling
 */

void direct_indexing(void) {
    int array[100];
    int *ptr = array;
    int index = 10;

    // Direct index, no manual scaling - COMPLIANT
    int *target = ptr + index;
    *target = 42;

    // Or use array notation - COMPLIANT
    array[index] = 42;
}

int main(void) {
    direct_indexing();
    return 0;
}
