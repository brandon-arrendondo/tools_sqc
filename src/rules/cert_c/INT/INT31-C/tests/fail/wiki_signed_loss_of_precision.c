/*
 * Rule: INT31-C
 * Source: wiki
 * Status: FAIL - Should trigger INT31-C violation
 */

#include <limits.h>

void func(void) {
  signed long int s_a = LONG_MAX;
  signed char sc = (signed char)s_a; /* Cast eliminates warning */
  /* ... */
}