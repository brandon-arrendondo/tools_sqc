/*
 * Rule: INT02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT02-C violation
 */

int si = -1;
unsigned ui = 1;
printf("%d\n", si < (int)ui);