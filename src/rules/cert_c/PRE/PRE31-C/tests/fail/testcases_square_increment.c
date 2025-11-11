/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Increment in unsafe SQUARE macro
 */

#define SQUARE(x) ((x) * (x))  /* UNSAFE */

void calculate(int n) {
    // Increment evaluated twice
    int result = SQUARE(++n);  // Line 11 - VIOLATION
    // Expands to: result = ((++n) * (++n));
}

int main(void) {
    calculate(5);
    return 0;
}
