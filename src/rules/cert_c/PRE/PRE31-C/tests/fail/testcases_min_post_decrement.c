/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Post-decrement in unsafe MIN macro
 */

#define MIN(a, b) ((a) < (b) ? (a) : (b))  /* UNSAFE */

void find_min(int x, int y) {
    // Post-decrement has side effect
    int min_val = MIN(x--, y);  // Line 11 - VIOLATION
}

int main(void) {
    find_min(5, 10);
    return 0;
}
