/*
 * Rule: DCL01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL01-C violation
 */

void f(void) {
  for (int i = 0; i < 10; i++) {
    long j;
    /* ... */
  }
}