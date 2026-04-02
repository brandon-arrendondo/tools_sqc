/*
 * Rule: MSC13-C
 * Status: FAIL - Variable declared but never used
 */

void f(void) {
    int x;  /* VIOLATION: x is never used */
}
