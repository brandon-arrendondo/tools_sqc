/*
 * Rule: MSC12-C
 * Status: FAIL - Expression with no side effects used as statement
 */

void f(int x, int y) {
    x + y;  /* VIOLATION: no-effect expression statement */
}
