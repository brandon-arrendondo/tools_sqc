/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Iterating over struct fields with pointer arithmetic
 */

struct data {
    int field1;
    int field2;
    int field3;
};

void process_struct(struct data *d) {
    int *ptr = &d->field1;

    // Iterate assuming fields are contiguous
    for (int i = 0; i < 3; i++) {
        *ptr = i;  // Line 18 - VIOLATION (ptr incremented below)
        ptr++;     // Line 19 - VIOLATION
    }
}

int main(void) {
    struct data d = {0};
    process_struct(&d);
    return 0;
}
