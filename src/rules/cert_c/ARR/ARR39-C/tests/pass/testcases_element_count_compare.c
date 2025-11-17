/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using element count in pointer comparison
 */

#define ARRAY_LEN 100

void safe_iteration(void) {
    short array[ARRAY_LEN];
    short *ptr = array;

    // Use element count, not sizeof - COMPLIANT
    while (ptr < array + ARRAY_LEN) {
        *ptr++ = 0;
    }
}

int main(void) {
    safe_iteration();
    return 0;
}
