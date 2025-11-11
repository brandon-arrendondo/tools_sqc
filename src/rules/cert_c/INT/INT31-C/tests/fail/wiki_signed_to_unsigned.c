/*
 * Rule: INT31-C
 * Source: wiki
 * Status: FAIL - Should trigger INT31-C violation
 */

#include <limits.h>

void func(signed int si) {
  /* Cast eliminates warning */
  unsigned int ui = (unsigned int)si;

  /* ... */
}

/* ... */

func(INT_MIN);