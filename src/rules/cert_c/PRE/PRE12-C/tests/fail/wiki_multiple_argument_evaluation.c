/*
 * Rule: PRE12-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE12-C violation
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))

void f(int n) {
  int m;
  m = ABS(++n);
  /* ... */
}