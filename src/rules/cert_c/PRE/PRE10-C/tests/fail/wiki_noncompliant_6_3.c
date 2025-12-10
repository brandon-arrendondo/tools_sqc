/*
 * Rule: PRE10-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE10-C violation
 *
 * This example shows a brace-wrapped multistatement macro that is not
 * wrapped in do-while(0). When followed by a semicolon, it creates an
 * empty statement that can break if-else structures.
 */

/* Non-compliant: uses braces but not do-while(0) */
#define SWAP_BRACES(x, y) { tmp = (x); (x) = (y); (y) = tmp; }

void do_something(void);

void test_brace_macro(void) {
    int x = 1, y = 2, tmp;
    if (x > y)
      SWAP_BRACES(x, y);   /* Creates: { ... }; (empty statement) */
    /* Error: else after empty statement */
}
