/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using sizeof to calculate element count, not for pointer arithmetic
 */

void element_count_calc(void) {
    long long data[30];
    long long *ptr = data;

    // Calculate element count - COMPLIANT
    size_t element_count = sizeof(data) / sizeof(data[0]);

    // Use element count in pointer arithmetic - COMPLIANT
    for (size_t i = 0; i < element_count; i++) {
        ptr[i] = i;
    }
}

int main(void) {
    element_count_calc();
    return 0;
}
