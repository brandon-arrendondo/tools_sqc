/*
 * Rule: PRE10-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE10-C violation
 *
 * This example shows a multistatement macro that expands to multiple
 * statements, causing only the first statement to be in the if block.
 */

/* Non-compliant: macro with multiple statements, not wrapped */
#define SET_VALUES tmp = x; x = y; y = tmp

void test_values(void) {
    int x = 1, y = 2, tmp;
    if (x > y)
      tmp = x;   /* Looks like one statement but is part of macro expansion */
    x = y;       /* This is always executed! Bug! */
    y = tmp;
}
