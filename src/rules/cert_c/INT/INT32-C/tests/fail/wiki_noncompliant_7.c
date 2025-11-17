/*
 * Rule: INT32-C
 * Source: wiki
 * Status: FAIL - Should trigger INT32-C violation
 */

void func(signed long s_a) {
  signed long result = -s_a;
  /* ... */
}