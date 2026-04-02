/*
 * Rule: MSC13-C
 * Status: FAIL - Array variable declared but never used
 */

void f(void) {
    int arr[10];  /* VIOLATION: arr is never used */
}
