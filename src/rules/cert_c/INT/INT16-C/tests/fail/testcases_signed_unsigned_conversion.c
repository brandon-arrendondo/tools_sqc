/*
 * Rule: INT16-C
 * Source: testcases
 * Status: FAIL - Signed-to-unsigned conversion without range check
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
