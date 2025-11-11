/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: Constant arguments have no side effects
 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))  /* UNSAFE */
#define SQUARE(x) ((x) * (x))  /* UNSAFE */

void constant_test(void) {
    // Constants have no side effects - COMPLIANT
    int max_val = MAX(10, 20);
    int sq = SQUARE(5);

    int x = 10;
    // Variable read (no side effect) - COMPLIANT
    int result = MAX(x, 15);
}

int main(void) {
    constant_test();
    return 0;
}
