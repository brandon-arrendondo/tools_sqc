/*
 * Rule: INT17-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT17-C violation
 */

const unsigned long mask = ~(ULONG_MAX >> 1);
unsigned long x;

/* Initialize x */

x |= mask;