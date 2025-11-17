/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single enum variable
 */

enum color { RED, GREEN, BLUE };

void enum_test(void) {
    enum color c = RED;
    enum color *ptr = &c;

    // Pointer arithmetic on single enum variable
    ptr++;  // Line 14 - VIOLATION
    *ptr = BLUE;  // Undefined behavior
}

int main(void) {
    enum_test();
    return 0;
}
