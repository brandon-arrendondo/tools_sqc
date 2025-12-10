/*
 * Rule: PRE02-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE02-C violation
 *
 * Non-compliant: Macro body is not fully parenthesized.
 * 81 / CUBE(i) expands to 81 / (i) * (i) * (i) = 243 instead of 3
 */

#define CUBE(X) (X) * (X) * (X)
int i = 3;
int a = 81 / CUBE(i);