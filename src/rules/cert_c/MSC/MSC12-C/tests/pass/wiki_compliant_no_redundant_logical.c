/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC12-C violation
 * Pattern: Non-redundant logical conditions
 */

void do_x(void);
void do_w(void);

void func(int a, int b, int c) {
    if (a == b) {  /* Compliant: no redundant sub-expression */
        do_x();
    }
    if (a == c) {  /* Compliant: no redundant sub-expression */
        do_w();
    }
}
