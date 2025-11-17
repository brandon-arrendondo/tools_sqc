/*
 * Rule: PRE01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE01-C violation
 */

#define CUBE(I) ( (I) * (I) * (I) )
int a = 81 / CUBE(2 + 1);