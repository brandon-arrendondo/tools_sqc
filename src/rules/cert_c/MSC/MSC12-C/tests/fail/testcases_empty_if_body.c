/*
 * Rule: MSC12-C
 * Status: FAIL - Empty if body is dead code
 */

void f(int x) {
    if (x > 0) {
        /* empty body — VIOLATION */
    }
}
