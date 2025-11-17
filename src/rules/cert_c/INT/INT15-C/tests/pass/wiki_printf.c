/*
 * Rule: INT15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT15-C violation
 */

#include <stdio.h>
#include <inttypes.h>

mytypedef_t x;

/* ... */

printf("%ju", (uintmax_t) x);