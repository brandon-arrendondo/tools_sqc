/*
 * Rule: MSC13-C
 * Status: FAIL - Variable initialized but never read
 */

void f(void) {
    int x = 42;  /* VIOLATION: x is never used */
}
