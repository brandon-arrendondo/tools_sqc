/*
 * Rule: MSC12-C
 * Status: FAIL - Empty for loop body
 */

void f(void) {
    int i;
    for (i = 0; i < 10; i++) {
        /* empty body — VIOLATION */
    }
}
