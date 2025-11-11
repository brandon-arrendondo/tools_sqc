/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Logical AND with increment in unsafe macro
 */

#define IS_VALID_RANGE(x, low, high) ((x) >= (low) && (x) <= (high))  /* UNSAFE */

void range_check(int val) {
    int lower = 0;

    // Increment in range check evaluated multiple times
    if (IS_VALID_RANGE(val, ++lower, 100)) {  // Line 13 - VIOLATION
        // lower incremented multiple times
    }
}

int main(void) {
    range_check(50);
    return 0;
}
