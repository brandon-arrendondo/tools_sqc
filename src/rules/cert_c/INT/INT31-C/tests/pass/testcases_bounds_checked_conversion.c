/*
 * Rule: INT31-C
 * Status: PASS - Conversion with bounds check before assignment
 */

#include <limits.h>
#include <stdio.h>

void f(long val) {
    if (val >= SHRT_MIN && val <= SHRT_MAX) {
        short s = (short)val;  /* Safe: bounds checked */
        printf("%d\n", s);
    }
}
