/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic across struct members with potential padding
 */

struct mixed {
    char c;
    int i;
    short s;
};

void access_with_padding(struct mixed *m) {
    char *ptr = &m->c;

    // Pointer arithmetic ignoring potential padding
    ptr += sizeof(char);  // Line 17 - VIOLATION (assumes no padding)
    *(int *)ptr = 100;  // Undefined behavior
}

int main(void) {
    struct mixed m = {'A', 10, 20};
    access_with_padding(&m);
    return 0;
}
