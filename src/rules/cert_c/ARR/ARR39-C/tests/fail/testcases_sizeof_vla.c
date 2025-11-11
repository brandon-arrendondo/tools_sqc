/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof on VLA for pointer arithmetic
 */

void vla_sizeof(int n) {
    int vla[n];
    int *ptr = vla;

    // sizeof(vla) returns bytes
    int *end = ptr + sizeof(vla);  // Line 12 - VIOLATION
    *(end - 1) = 999;
}

int main(void) {
    vla_sizeof(20);
    return 0;
}
