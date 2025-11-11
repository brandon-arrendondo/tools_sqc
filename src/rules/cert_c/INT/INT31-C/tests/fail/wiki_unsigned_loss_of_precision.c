/*
 * Rule: INT31-C
 * Source: wiki
 * Status: FAIL - Should trigger INT31-C violation
 */

#include <limits.h>

void func(void) {
  unsigned long int u_a = ULONG_MAX;
  unsigned char uc = (unsigned char)u_a; /* Cast eliminates warning */
  /* ... */
}