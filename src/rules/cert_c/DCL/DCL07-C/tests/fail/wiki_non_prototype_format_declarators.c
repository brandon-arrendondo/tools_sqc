/*
 * Rule: DCL07-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL07-C violation
 */

int max(a, b)
int a, b;
{
  return a > b ? a : b;
}