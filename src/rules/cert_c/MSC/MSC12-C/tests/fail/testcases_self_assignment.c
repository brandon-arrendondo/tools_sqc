/*
 * Rule: MSC12-C
 * Status: FAIL - Self-assignment is meaningless
 */

void f(int x) {
    x = x;  /* VIOLATION: self-assignment */
}
