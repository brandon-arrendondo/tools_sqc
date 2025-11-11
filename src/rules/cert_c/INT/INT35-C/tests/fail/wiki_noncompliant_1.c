/*
 * Rule: INT35-C
 * Source: wiki
 * Status: FAIL - Should trigger INT35-C violation
 */

#include <limits.h>
 
unsigned int pow2(unsigned int exp) {
  if (exp >= sizeof(unsigned int) * CHAR_BIT) {
    /* Handle error */
  }
  return 1 << exp;
}