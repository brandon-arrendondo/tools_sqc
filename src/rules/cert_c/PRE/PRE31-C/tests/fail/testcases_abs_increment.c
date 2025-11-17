/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Using increment operator in unsafe ABS macro
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

void func(int n) {
    // Increment has side effect - evaluated 3 times
    int m = ABS(++n);  // Line 11 - VIOLATION
    // Expands to: m = (((++n) < 0) ? -(++n) : (++n));
}

int main(void) {
    func(5);
    return 0;
}
