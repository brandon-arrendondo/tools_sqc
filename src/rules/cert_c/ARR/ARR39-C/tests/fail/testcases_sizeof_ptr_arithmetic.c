/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Pointer arithmetic with sizeof in expression
 */

void sizeof_in_expr(void) {
    double values[70];
    double *start = values;

    // Complex expression with sizeof
    double *mid = start + (sizeof(values) / 2);  // Line 12 - VIOLATION
    *mid = 5.5;
}

int main(void) {
    sizeof_in_expr();
    return 0;
}
