/*
 * Rule: MSC12-C
 * Status: FAIL - Empty while body
 */

void f(int *flag) {
    while (*flag) {
        /* empty loop body — VIOLATION */
    }
}
