/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic across nested struct members
 */

struct inner {
    int x;
    int y;
};

struct outer {
    struct inner in;
    int z;
};

void access_nested(struct outer *obj) {
    int *ptr = &obj->in.x;

    // Pointer arithmetic assuming contiguous layout
    ptr += 2;  // Line 22 - VIOLATION
    *ptr = 99;
}

int main(void) {
    struct outer o = {{1, 2}, 3};
    access_nested(&o);
    return 0;
}
