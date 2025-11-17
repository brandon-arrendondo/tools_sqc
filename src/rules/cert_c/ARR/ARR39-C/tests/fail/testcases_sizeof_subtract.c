/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Subtracting sizeof() from pointer causes double-scaling
 */

void subtract_sizeof(void) {
    long buffer[60];
    long *end = buffer + 60;

    // Subtracting sizeof() - double-scaling
    long *ptr = end - sizeof(buffer);  // Line 12 - VIOLATION
    *ptr = 100L;
}

int main(void) {
    subtract_sizeof();
    return 0;
}
