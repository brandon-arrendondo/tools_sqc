/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Side effects in both arguments of unsafe MAX macro
 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))  /* UNSAFE */

void compare(int x, int y) {
    // Both arguments have side effects
    int max_val = MAX(++x, --y);  // Line 11 - VIOLATION
}

int main(void) {
    compare(10, 20);
    return 0;
}
