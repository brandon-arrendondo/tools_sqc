/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Attempting pointer arithmetic on struct with bitfields
 */

struct flags {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int flag3 : 1;
    unsigned int value : 29;
};

void bitfield_access(struct flags *f) {
    // Note: can't take address of bitfield, but can do arithmetic on struct
    unsigned int *ptr = (unsigned int *)f;

    // Pointer arithmetic treating struct as array
    ptr++;  // Line 19 - VIOLATION
    *ptr = 0xFF;  // Undefined behavior
}

int main(void) {
    struct flags f = {1, 0, 1, 100};
    bitfield_access(&f);
    return 0;
}
