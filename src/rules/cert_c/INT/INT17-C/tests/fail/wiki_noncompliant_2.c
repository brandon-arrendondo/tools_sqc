/*
 * Rule: INT17-C
 * Source: wiki
 * Status: FAIL - Should trigger INT17-C violation
 */

const unsigned long mask = 0x80000000;
unsigned long x;

/* Initialize x */

x |= mask;