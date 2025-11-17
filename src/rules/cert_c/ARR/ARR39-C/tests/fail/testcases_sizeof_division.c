/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof result incorrectly in pointer arithmetic
 */

void sizeof_div_issue(void) {
    long long data[30];
    long long *ptr = data;

    // sizeof returns bytes, but used as if element count
    for (size_t i = 0; i < sizeof(data); i++) {
        ptr[i] = i;  // Line 12 - VIOLATION (i ranges to byte count)
    }
}

int main(void) {
    sizeof_div_issue();
    return 0;
}
