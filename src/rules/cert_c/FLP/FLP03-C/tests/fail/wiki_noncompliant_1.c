/*
 * Rule: FLP03-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP03-C violation
 */

void fpOper_noErrorChecking(void) {
  /* ... */
  double a = 1e-40, b, c = 0.1;
  float x = 0, y;
  /* Inexact and underflows */
  y = a;
  /* Divide-by-zero operation */
  b = y / x;
  /* Inexact (loss of precision) */
  c = sin(30) * a;
  /* ... */
}