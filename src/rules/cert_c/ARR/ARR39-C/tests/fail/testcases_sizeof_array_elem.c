/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Multiplying by sizeof(array element) in pointer arithmetic
 */

void sizeof_element(void) {
    float array[80];
    float *ptr = array;
    int offset = 20;

    // Scaling offset by element size
    float *target = ptr + (offset * sizeof(float));  // Line 13 - VIOLATION
    *target = 3.14f;
}

int main(void) {
    sizeof_element();
    return 0;
}
