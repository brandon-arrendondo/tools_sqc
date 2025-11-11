/*
 * Rule: PRE01-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE01-C violation
 */

int a = 81 / (2 + 1 * 2 + 1 * 2 + 1);  /* Evaluates to 11 */