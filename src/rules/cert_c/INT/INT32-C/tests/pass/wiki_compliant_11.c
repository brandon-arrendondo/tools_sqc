/*
 * Rule: INT32-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT32-C violation
 */

#include <limits.h>
 
void func(signed long s_a) {
  signed long result;
  if (s_a == LONG_MIN) {
    /* Handle error */
  } else {
    result = -s_a;
  }
  /* ... */
}