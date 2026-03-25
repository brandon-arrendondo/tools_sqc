/*
 * Rule: INT16-C
 * Source: testcases
 * Status: PASS - Known limitation: signed-to-unsigned conversion not detected
 * TODO: Move to fail/ when implemented (see PLAN.md)
 */

#include <limits.h>

/* Negative value assigned to unsigned */
void negative_to_unsigned(int val) {
    unsigned int uval = val;
    (void)uval;
}

/* Signed return assigned to unsigned without check */
unsigned int convert_unchecked(int x) {
    return x;
}
