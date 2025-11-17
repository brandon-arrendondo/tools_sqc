/*
 * Rule: PRE02-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE02-C violation
 */

int a = ((81 / i) * i) * i);  /* Evaluates to 243 */