/*
 * Rule: INT35-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT35-C violation
 */

#include <limits.h>

unsigned int pow2(unsigned int exp) {
  if (exp >= UINT_WIDTH) {
    /* Handle error */
  }
  return 1 << exp;
}