/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: No side effects in MAX macro arguments
 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))  /* UNSAFE */

void compare(int x, int y) {
    // No side effects - COMPLIANT
    int max_val = MAX(x, y);

    // Side effects after macro call
    ++x;
    ++y;
}

int main(void) {
    compare(10, 20);
    return 0;
}
