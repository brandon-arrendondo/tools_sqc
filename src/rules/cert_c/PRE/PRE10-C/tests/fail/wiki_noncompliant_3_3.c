/*
 * Rule: PRE10-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE10-C violation
 */

int x, y, z, tmp;
if (z == 0)
  tmp = x;
x = y;
y = tmp;