/*
 * Rule: INT32-C
 * Source: wiki
 * Status: FAIL - Should trigger INT32-C violation
 */

void func(signed int si_a, signed int si_b) {
  signed int diff = si_a - si_b;
  /* ... */
}