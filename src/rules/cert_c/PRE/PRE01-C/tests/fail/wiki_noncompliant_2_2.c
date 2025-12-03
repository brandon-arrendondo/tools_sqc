/*
 * Rule: PRE01-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE01-C violation
 *
 * Non-compliant macro with unparenthesized parameters.
 * CUBE(2 + 1) expands to (2 + 1 * 2 + 1 * 2 + 1) = 11 instead of 27
 */

#define CUBE(I) (I * I * I)
int a = 81 / CUBE(2 + 1);