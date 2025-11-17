/*
 * Rule: INT02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT02-C violation
 */

unsigned short x = 45000, y = 50000;
unsigned int z = x * (unsigned int)y;