/*
 * Rule: DCL19-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL19-C violation
 */

unsigned int count = 0;

void counter() {
  if (count++ > MAX_COUNT) return;
  /* ... */

}