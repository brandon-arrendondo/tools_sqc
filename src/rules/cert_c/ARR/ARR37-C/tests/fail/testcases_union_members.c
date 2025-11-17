/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on union member
 */

union data {
    int i;
    float f;
    char c;
};

void union_access(union data *u) {
    char *ptr = &u->c;

    // Pointer arithmetic on union member
    ptr++;  // Line 17 - VIOLATION
    *ptr = 'X';  // Undefined behavior
}

int main(void) {
    union data u = {.i = 100};
    union_access(&u);
    return 0;
}
