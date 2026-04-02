/*
 * Rule: MSC13-C
 * Status: PASS - Variable passed via address-of operator
 */

void fill(int *out) {
    *out = 42;
}

void f(void) {
    int x = 0;
    fill(&x);
}
