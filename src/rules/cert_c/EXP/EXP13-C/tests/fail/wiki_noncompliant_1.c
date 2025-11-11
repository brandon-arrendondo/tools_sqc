/*
 * Rule: EXP13-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP13-C violation
 */

int a = 2;
int b = 2;
int c = 2;
/* ... */
if (a < b < c) /* Misleading; likely bug */
/* ... */
if (a == b == c) /* Misleading; likely bug */