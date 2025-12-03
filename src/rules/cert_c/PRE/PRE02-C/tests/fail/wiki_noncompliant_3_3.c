/*
 * Rule: PRE02-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE02-C violation
 *
 * Non-compliant: SQUARE macro body not fully parenthesized.
 * 81 / SQUARE(3) expands to 81 / 3 * 3 = 81 instead of 9
 */

#define SQUARE(X) (X) * (X)
int a = 81 / SQUARE(3);