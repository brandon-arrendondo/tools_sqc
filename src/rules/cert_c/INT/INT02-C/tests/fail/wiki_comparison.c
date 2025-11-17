/*
 * Rule: INT02-C
 * Source: wiki
 * Status: FAIL - Should trigger INT02-C violation
 */

int si = -1;
unsigned int ui = 1;
printf("%d\n", si < ui);