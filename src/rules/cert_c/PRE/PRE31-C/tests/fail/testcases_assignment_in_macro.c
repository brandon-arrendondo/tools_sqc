/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Assignment in unsafe macro argument
 */

#define DOUBLE(x) ((x) + (x))  /* UNSAFE */

void test_assign(int a) {
    int b = 10;

    // Assignment has side effect - evaluated twice
    int result = DOUBLE(b = a);  // Line 13 - VIOLATION
}

int main(void) {
    test_assign(5);
    return 0;
}
