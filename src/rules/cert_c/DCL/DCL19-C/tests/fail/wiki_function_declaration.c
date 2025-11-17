/*
 * Rule: DCL19-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL19-C violation
 */

int f(int i) {
  /* Function definition */
}

int g(int i) {
  int j = f(i);
  /* ... */
}