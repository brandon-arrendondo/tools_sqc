/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Pre-increment in unsafe MAX macro
 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))  /* UNSAFE */

void compare(int x, int y) {
    // Pre-increment evaluated multiple times
    int max_val = MAX(++x, y);  // Line 11 - VIOLATION
}

int main(void) {
    compare(10, 20);
    return 0;
}
