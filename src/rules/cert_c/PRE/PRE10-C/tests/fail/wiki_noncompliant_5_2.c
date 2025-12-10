/*
 * Rule: PRE10-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE10-C violation
 *
 * This example demonstrates how a multistatement macro used in an
 * if-else statement can cause the else branch to become unreachable
 * or cause parsing errors.
 */

/* Non-compliant: SWAP macro without do-while wrapper */
#define SWAP(x, y) tmp = x; x = y; y = tmp

void do_something(void);

void test_if_else(void) {
    int x = 1, y = 2, tmp;
    if (x > y)
      SWAP(x, y);          /* Bug: disrupts the if-else structure */
    else
      do_something();      /* May cause parse errors or wrong behavior */
}
