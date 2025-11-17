/*
 * Rule: PRE02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE02-C violation
 */

#define CUBE(X) ((X) * (X) * (X))
int i = 3;
int a = 81 / CUBE(i);