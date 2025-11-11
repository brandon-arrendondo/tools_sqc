/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Increment in unsafe SWAP macro
 */

#define SWAP(a, b) do { typeof(a) tmp = (a); (a) = (b); (b) = tmp; } while(0)  /* UNSAFE */

void swap_values(int x, int y) {
    // Increment has side effect - evaluated multiple times
    SWAP(++x, y);  // Line 11 - VIOLATION
}

int main(void) {
    swap_values(5, 10);
    return 0;
}
