/*
 * Rule: INT33-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT33-C violation
 */

#include <limits.h>
 
void func(signed long s_a, signed long s_b) {
  signed long result;
  if ((s_b == 0 ) || ((s_a == LONG_MIN) && (s_b == -1))) {
    /* Handle error */
  } else {
    result = s_a % s_b;
  }
  /* ... */
}