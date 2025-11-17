/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Increment in unsafe CLAMP macro
 */

#define CLAMP(x, low, high) (((x) < (low)) ? (low) : (((x) > (high)) ? (high) : (x)))  /* UNSAFE */

void clamp_value(int val) {
    // Increment evaluated multiple times
    int result = CLAMP(++val, 0, 100);  // Line 11 - VIOLATION
}

int main(void) {
    clamp_value(50);
    return 0;
}
