/*
 * Rule: PRE10-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE10-C violation
 *
 * This example shows a multistatement macro not wrapped in do-while.
 * When used in an if statement without braces, only the first statement
 * is part of the if block.
 */

/* Non-compliant: SWAP macro without do-while wrapper */
#define SWAP(x, y) tmp = x; x = y; y = tmp

void test_swap(void) {
    int x = 1, y = 2, z = 0, tmp;
    if (z == 0)
      SWAP(x, y);  /* Bug: only "tmp = x" is in the if block! */
}
