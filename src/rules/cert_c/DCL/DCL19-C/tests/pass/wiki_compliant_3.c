/*
 * Rule: DCL19-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL19-C violation
 */

static int f(int i) {
  /* Function definition */
}

int g(int i) {
  int j = f(i);
  /* ... */
}