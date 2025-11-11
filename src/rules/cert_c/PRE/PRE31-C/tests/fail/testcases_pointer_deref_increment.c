/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Pointer dereference with increment in unsafe macro
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

void pointer_op(int *ptr) {
    // *ptr++ has side effect - evaluated multiple times
    int result = ABS(*ptr++);  // Line 11 - VIOLATION
}

int main(void) {
    int data[] = {-5, 10, -3};
    pointer_op(data);
    return 0;
}
