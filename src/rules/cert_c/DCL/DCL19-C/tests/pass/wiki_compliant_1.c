/*
 * Rule: DCL19-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL19-C violation
 */

void counter() {
  static unsigned int count = 0;
  if (count++ > MAX_COUNT) return;
  /* ... */

}