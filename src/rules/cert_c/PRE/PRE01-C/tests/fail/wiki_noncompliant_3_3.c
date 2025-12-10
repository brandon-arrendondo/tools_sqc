/*
 * Rule: PRE01-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE01-C violation
 *
 * Non-compliant macro: SQUARE lacks parentheses around parameter.
 * SQUARE(1+2) expands to (1+2*1+2) = 5 instead of 9
 */

#define SQUARE(X) (X * X)
int a = 81 / SQUARE(1 + 2);  /* Evaluates to 5 incorrectly */