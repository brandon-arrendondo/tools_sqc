/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: Pure function without side effects (PRE31-C-EX1)
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

// Pure function - no side effects
int square(int x) {
    return x * x;
}

void calc(int n) {
    // Pure function without side effects - COMPLIANT (PRE31-C-EX1)
    int result = ABS(square(n));
}

int main(void) {
    calc(5);
    return 0;
}
