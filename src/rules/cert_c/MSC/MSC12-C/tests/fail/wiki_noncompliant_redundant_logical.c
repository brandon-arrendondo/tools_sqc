/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC12-C violation
 * Pattern: Redundant sub-expressions in logical operators
 */

void do_x(void);
void do_w(void);

void func(int a, int b, int c) {
    if (a == b && a == b) {  /* Noncompliant: second condition always same */
        do_x();
    }
    if (a == c || a == c) {  /* Noncompliant: second condition always same */
        do_w();
    }
}
