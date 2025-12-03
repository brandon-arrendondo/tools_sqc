/*
 * Rule: PRE02-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE02-C violation
 *
 * Non-compliant: MUL macro body not fully parenthesized.
 * 10 / MUL(2, 3) expands to 10 / 2 * 3 = 15 instead of 10 / 6 ≈ 1
 */

#define MUL(X, Y) (X) * (Y)
int a = 10 / MUL(2, 3);