/*
 * Rule: INT02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT02-C violation
 */

int si = /* Some signed value */;
unsigned ui = /* Some unsigned value */;
printf("%d\n", (si < 0 || (unsigned)si < ui));