/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: Side effect separated from macro call
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

void func(int n) {
    // Increment before macro call - COMPLIANT
    ++n;
    int m = ABS(n);  // No side effect in argument
}

int main(void) {
    func(5);
    return 0;
}
