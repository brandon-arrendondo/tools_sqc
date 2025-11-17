/*
 * Rule: INT31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT31-C violation
 */

#include <limits.h>

void func(signed int si) {
  unsigned int ui;
  if (si < 0) {
    /* Handle error */
  } else {
    ui = (unsigned int)si;  /* Cast eliminates warning */
  }
  /* ... */
}
/* ... */

func(INT_MIN + 1);