/*
 * Rule: INT15-C
 * Source: wiki
 * Status: FAIL - Should trigger INT15-C violation
 */

#include <stdio.h>

mytypedef_t x;

/* ... */

printf("%llu", (unsigned long long) x);