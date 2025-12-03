/*
 * Rule: PRE02-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE02-C violation
 *
 * Non-compliant: ADD macro body not fully parenthesized.
 * a * ADD(2, 3) expands to a * 2 + 3 instead of a * (2 + 3)
 */

#define ADD(X, Y) (X) + (Y)
int a = 5 * ADD(2, 3);