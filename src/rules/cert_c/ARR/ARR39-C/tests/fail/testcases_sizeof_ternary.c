/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: sizeof used in ternary expression for pointer arithmetic
 */

void ternary_sizeof(int flag) {
    float data[60];
    float *ptr = data;

    // sizeof in ternary for offset
    float *target = ptr + (flag ? sizeof(data) : 10);  // Line 12 - VIOLATION
    *target = 1.0f;
}

int main(void) {
    ternary_sizeof(1);
    return 0;
}
