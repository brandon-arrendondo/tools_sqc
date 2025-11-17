/*
 * Rule: INT31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT31-C violation
 */

#include <limits.h>

void func(void) {
  signed long int s_a = LONG_MAX;
  signed char sc;
  if ((s_a < SCHAR_MIN) || (s_a > SCHAR_MAX)) {
    /* Handle error */
  } else {
    sc = (signed char)s_a; /* Use cast to eliminate warning */
  }
  /* ... */
}