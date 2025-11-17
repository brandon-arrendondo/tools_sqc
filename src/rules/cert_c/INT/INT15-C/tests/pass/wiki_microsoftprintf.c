/*
 * Rule: INT15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT15-C violation
 */

#include <stdio.h>
#include <inttypes.h>

mytypedef_t x;

/* ... */

#ifdef _MSC_VER
  printf("%llu", (uintmax_t) x);
#else
  printf("%ju", (uintmax_t) x);
#endif